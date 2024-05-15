/////////////////////////////////////////////////////////////////////////////
//                         Single Threaded Networking
//
// This file is distributed under the MIT License. See the LICENSE file
// for details.
/////////////////////////////////////////////////////////////////////////////


#ifdef __EMSCRIPTEN__

// The Servers are incompatible with web sockets in the browser, so disable them

#else


#include "Server.h"
#include "GameOrchestrator.h"
#include <iostream>
#include <fstream>


#include <random>
#include <sstream>
#include <set>
#include <cstdlib>
#include <ctime>
#include <algorithm>


#include <boost/asio.hpp>
#include <boost/beast.hpp>


using namespace std::string_literals;
using networking::Message;
using networking::Server;
using networking::ServerImpl;
using networking::ServerImplDeleter;


// Move to a separate file after
std::string getHTTPMessage(const char* htmlLocation) {
  if (access(htmlLocation, R_OK ) != -1) {
    std::ifstream infile{htmlLocation};
    return std::string{std::istreambuf_iterator<char>(infile),
                       std::istreambuf_iterator<char>()};

  }

  std::cerr << "Unable to open HTML index file:\n"
            << htmlLocation << "\n";
  std::exit(-1);
};

std::unordered_map<std::string, std::string> httpPathToFileMapping = {
    // Able to go to the original webchat example
    {"webchat", getHTTPMessage("../../web-socket-networking/webchat.html")},

    // Social gaming paths
    {"landingpage", getHTTPMessage("../../frontend/html/landingpage.html")},
    {"creategamesession", getHTTPMessage("../../frontend/html/creategamesession.html")}, //css hidden proerty on a boolean
    {"joinroom", getHTTPMessage("../../frontend/html/joinroom.html")},
    {"chatgame", getHTTPMessage("../../web-socket-networking/webchat.html")},
    {"rps", getHTTPMessage("../../frontend/html/chatgame.html")},
    {"player", getHTTPMessage("../../frontend/html/player.html")},
};

//map to keep track of the randomcodes as keys mapped to the respective game sessions.
std::unordered_map<std::string, std::string> randomCodeToGameSession = {};

//A record of all generated codes
std::set<std::string> generatedCodes;

//Generate a random code for a new game session
//Ensures unique code generation until the server survives
std::string 
generateRandomCode() {
    // std::random_device rd;
    // std::mt19937 gen(rd());
    // std::uniform_int_distribution<int> 
    //   distribution(10000000, 99999999);
    // int randomCode = distribution(gen);

    // std::ostringstream oss;
    // oss << randomCode;
    //return oss.str();
    const int codeLength = 8;
    const char* charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const int charsetSize = strlen(charset);

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    while (true) {
        std::string code;
        for (int i = 0; i < codeLength; ++i) {
            code += charset[std::rand() % charsetSize];
        }

        if (generatedCodes.insert(code).second) {
            return code;
        }
    }
}

// Rewriting to make it better testable
// adding edge case checks
// TODO: replace std::string with "tiny types"
bool
isValidGameCode(std::string gameCode){
  if(gameCode.length()!=8){
    return false;
  }

  bool allAlphanumeric = std::all_of(gameCode.begin(), gameCode.end(), [](char c) {
        return std::isalnum(c);
  });

  if (!allAlphanumeric) {
    return false;
  }

  auto it = randomCodeToGameSession.find(gameCode);
  //check if no entry of the game code in the map
  return it!=randomCodeToGameSession.end();
}

void
mapCodeToGameSession( std::string gameCode, std::string target_str){
  randomCodeToGameSession[gameCode] = target_str;
}

//delete the game code entry from the map 'randomCodeToGameSession'
void
removeMappedGameCode(std::string gameCode){}

namespace networking {


/////////////////////////////////////////////////////////////////////////////
// Private Server API
/////////////////////////////////////////////////////////////////////////////


class Channel;


class ServerImpl {
public:

  ServerImpl(Server& server, unsigned short port, std::string httpMessage)
   : server{server},
     endpoint{boost::asio::ip::tcp::v4(), port},
     acceptor{ioContext, endpoint},
     httpMessage{std::move(httpMessage)} {
    listenForConnections();
  }

  void listenForConnections();
  void registerChannel(Channel& channel);
  void reportError(std::string_view message);

  using ChannelMap =
    std::unordered_map<Connection, std::shared_ptr<Channel>, ConnectionHash>;

  Server& server;
  const boost::asio::ip::tcp::endpoint endpoint;
  boost::asio::io_context ioContext{};
  boost::asio::ip::tcp::acceptor acceptor;
  boost::beast::http::string_body::value_type httpMessage;

  ChannelMap channels;
  std::deque<Message> incoming;
};


/////////////////////////////////////////////////////////////////////////////
// Channels (connections private to the implementation)
/////////////////////////////////////////////////////////////////////////////


class Channel : public std::enable_shared_from_this<Channel> {
public:
  Channel(boost::asio::ip::tcp::socket socket, ServerImpl& serverImpl)
    : connection{reinterpret_cast<uintptr_t>(this)},
      serverImpl{serverImpl},
      websocket{std::move(socket)},
      readBuffer{serverImpl.incoming}
      { }

  void start(boost::beast::http::request<boost::beast::http::string_body>& request);
  void send(std::string outgoing);
  void disconnect();

  [[nodiscard]] Connection getConnection() const noexcept { return connection; }

private:
  void readMessage();
  void afterWrite(std::error_code errorCode, std::size_t size);

  bool disconnected = false;
  Connection connection;
  ServerImpl &serverImpl;

  boost::beast::flat_buffer streamBuf{};
  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> websocket;

  std::deque<Message> &readBuffer;
  std::deque<std::string> writeBuffer;
};

}

using networking::Channel;


void
Channel::start(boost::beast::http::request<boost::beast::http::string_body>& request) {
  auto self = shared_from_this();
  websocket.async_accept(request,
    [this, self] (std::error_code errorCode) {
      if (!errorCode) {
        serverImpl.registerChannel(*this);
        self->readMessage();
      } else {
        serverImpl.server.disconnect(connection);
      }
    });
}


void
Channel::disconnect() {
  disconnected = true;
  boost::beast::error_code ec;
  websocket.close(boost::beast::websocket::close_reason{}, ec);
}


void
Channel::send(std::string outgoing) {
  if (outgoing.empty()) {
    return;
  }
  writeBuffer.push_back(std::move(outgoing));

  if (1 < writeBuffer.size()) {
    // Note, multiple writes will be chained within asio via `continueSending`,
    // so that callback should be used instead of directly invoking async_write
    // again.
    return;
  }

  websocket.async_write(boost::asio::buffer(writeBuffer.front()),
    [this, self = shared_from_this()] (auto errorCode, std::size_t size) {
      afterWrite(errorCode, size);
    });
}


void
Channel::afterWrite(std::error_code errorCode, std::size_t /*size*/) {
  if (errorCode) {
    if (!disconnected) {
      serverImpl.server.disconnect(connection);
    }
    return;
  }

  writeBuffer.pop_front();

  // Continue asynchronously processing any further messages that have been
  // sent.
  if (!writeBuffer.empty()) {
    websocket.async_write(boost::asio::buffer(writeBuffer.front()),
      [this, self = shared_from_this()] (auto errorCode, std::size_t size) {
        afterWrite(errorCode, size);
      });
  }
}


void
Channel::readMessage() {
  auto self = shared_from_this();
  websocket.async_read(streamBuf,
    [this, self] (auto errorCode, std::size_t /*size*/) {
      if (!errorCode) {
        auto message = boost::beast::buffers_to_string(streamBuf.data());
        readBuffer.push_back({connection, std::move(message)});
        streamBuf.consume(streamBuf.size());
        this->readMessage();
      } else if (!disconnected) {
        serverImpl.server.disconnect(connection);
      }
    });
}


////////////////////////////////////////////////////////////////////////////////
// Basic HTTP Request Handling
////////////////////////////////////////////////////////////////////////////////


class HTTPSession : public std::enable_shared_from_this<HTTPSession> {
public:
  explicit HTTPSession(ServerImpl& serverImpl)
    : serverImpl{serverImpl},
      socket{serverImpl.ioContext}
      { }

  void start();
  void handleRequest();

  boost::asio::ip::tcp::socket & getSocket() { return socket; }

private:
  ServerImpl &serverImpl;
  boost::asio::ip::tcp::socket socket;
  boost::beast::flat_buffer streamBuf{};
  boost::beast::http::request<boost::beast::http::string_body> request;
};


void
HTTPSession::start() {
  boost::beast::http::async_read(socket, streamBuf, request,
    [this, session = this->shared_from_this()]
    (std::error_code ec, std::size_t /*bytes*/) {
      if (ec) {
        serverImpl.reportError("Error reading from HTTP stream.");

      } else if (boost::beast::websocket::is_upgrade(request)) {
        auto channel = std::make_shared<Channel>(std::move(socket), serverImpl);
        channel->start(request);

      } else {
        session->handleRequest();
      }
    });
}

std::string extractFirstValueFromRequest(std::string_view url) {
    // Use cases -
    // "/" -> "landingpage"
    // "/test" -> "test"
    // "/test1/test2" -> "test1"
    // Note - currently as constructed impossible to receive an empty string

    auto defaultValue = "landingpage";
    size_t firstSlash = url.find('/');
    if (firstSlash != std::string::npos) {
        size_t secondSlash = url.find('/', firstSlash + 1);
        if (secondSlash != std::string::npos) {
            return std::string(url.substr(firstSlash + 1, secondSlash - firstSlash - 1));
        }
    }

    auto firstValue = url.substr(1);

    return firstValue.empty() ? defaultValue : std::string(firstValue);
}

std::string_view toStdStringView(boost::beast::string_view beastView) {
    return std::string_view(beastView.data(), beastView.size());
}

//Customize the Player's lobby page URL
std::string addToPlayerURL(std::string_view url){
    //size_t questionExp = url.find("?");
    size_t andExp = url.find("&");

    size_t firstEqualExp = url.find("=");
    size_t secondEqualExp = url.find("=", firstEqualExp+1);

    //game Code:
    std::string gameCode = (std::string)url.substr(secondEqualExp + 1);
    
    //check if valid game Code::
    if(!isValidGameCode(gameCode)){
      return (std::string)"/joinroom/invalidcode";
    }

    //Player Name:
    std::string playerName = (std::string)url.substr(
      firstEqualExp + 1, andExp - firstEqualExp - 1);
    
    std::string redirectToPath = "/player/" + playerName + "/" + gameCode;

    return redirectToPath;
}

void
HTTPSession::handleRequest() {
  auto send = [this, session = this->shared_from_this()] (auto&& response) {
    using Response = typename std::decay<decltype(response)>::type;
    auto sharedResponse =
      std::make_shared<Response>(std::forward<decltype(response)>(response));

    boost::beast::http::async_write(socket, *sharedResponse,
      [this, session, sharedResponse] (std::error_code ec, std::size_t /*bytes*/) {
        if (ec) {
          session->serverImpl.reportError("Error writing to HTTP stream");
          socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send);
        } else if (sharedResponse->need_eof()) {
          // This signifies a deliberate close
          boost::system::error_code ec;
          socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
          if (ec) {
            session->serverImpl.reportError("Error closing HTTP stream");
          }
        } else {
          session->start();
        }
      });
  };

  auto const badRequest =
    [&request = this->request] (std::string_view why) {
    boost::beast::http::response<boost::beast::http::string_body> result {
      boost::beast::http::status::bad_request,
      request.version()
    };
    result.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    result.set(boost::beast::http::field::content_type, "text/html");
    result.keep_alive(request.keep_alive());
    result.body() = why;
    result.prepare_payload();
    return result;
  };

  if (auto method = request.method();
      method != boost::beast::http::verb::get
      && method != boost::beast::http::verb::head) {
    send(badRequest("Unknown HTTP-method"));
  }

  //moved the extractFirstValue function outside the handleRequest()

  boost::beast::string_view complete_request_path = request.target();
  //convert boost::beast::string_view to string_view

  std::string request_key = extractFirstValueFromRequest(toStdStringView(complete_request_path));

  bool joinRoomAttempt = complete_request_path.find("joinroom?") != std::string::npos;

  if(joinRoomAttempt){
    request_key = "player";
  }

  //moving addtoplayerURL function outside this function
  //making it testable
  
  auto shouldServeHTTPRequest = [](const std::string& target) {
    return httpPathToFileMapping.find(target) != httpPathToFileMapping.end();
  };

  if (!shouldServeHTTPRequest(request_key )) {
    send(badRequest("Illegal request-target"));
  }

  boost::beast::http::string_body::value_type body = httpPathToFileMapping[request_key];


  // if(target_str.find("joinsession") != std::string::npos){
  //   // check map if session exists
  //   // add particiapnt to the game session
  // }


  auto addResponseMetaData =
    [bodySize = body.size(), &request = this->request] (auto& response) {
    response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(boost::beast::http::field::content_type, "text/html");
    response.content_length(bodySize);
    response.keep_alive(request.keep_alive());
  };



  if (request.method() == boost::beast::http::verb::head) {
    boost::beast::http::response<boost::beast::http::empty_body> result {
      boost::beast::http::status::ok,
      request.version()
    };
    addResponseMetaData(result);
    send(std::move(result));

  } 
  
  else if(complete_request_path.find("startsession") != std::string::npos){
    // Check if it's a createGameSession Request
    // Redirect to a game session with a unique id
    boost::beast::http::response<boost::beast::http::string_body> result {
      std::piecewise_construct,
      std::make_tuple(std::move(body)),
      std::make_tuple(boost::beast::http::status::see_other, request.version())
    };
    std::string randomCode = generateRandomCode();
    mapCodeToGameSession(randomCode, request_key);
    result.set(boost::beast::http::field::location, "/" + request_key + "/activesession/" + randomCode);
    addResponseMetaData(result);
    send(std::move(result));
  }
  else if (joinRoomAttempt){
    boost::beast::http::response<boost::beast::http::string_body> result {
      std::piecewise_construct,
      std::make_tuple(std::move(body)),
      std::make_tuple(boost::beast::http::status::see_other, request.version())
    };
    std::string redirectToPath = addToPlayerURL(toStdStringView(complete_request_path));
    result.set(boost::beast::http::field::location, redirectToPath);
    addResponseMetaData(result);
    send(std::move(result));
  }

  else {
    // Respond to GET
    boost::beast::http::response<boost::beast::http::string_body> result {
      std::piecewise_construct,
      std::make_tuple(std::move(body)),
      std::make_tuple(boost::beast::http::status::ok, request.version())
    };
    addResponseMetaData(result);
    send(std::move(result));
  }
}


/////////////////////////////////////////////////////////////////////////////
// Hidden Server implementation
/////////////////////////////////////////////////////////////////////////////


void
ServerImpl::listenForConnections() {
  auto session =
    std::make_shared<HTTPSession>(*this);

  acceptor.async_accept(session->getSocket(),
    [this, session] (auto errorCode) {
      if (!errorCode) {
        session->start();
      } else {
        reportError("Fatal error while accepting");
      }
      this->listenForConnections();
    });
}


void
ServerImpl::registerChannel(Channel& channel) {
  auto connection = channel.getConnection();
  channels[connection] = channel.shared_from_this();
  server.connectionHandler->handleConnect(connection);
}


void
ServerImpl::reportError(std::string_view /*message*/) {
  // Swallow errors....
}

void
ServerImplDeleter::operator()(ServerImpl* serverImpl) {
  // NOTE: This is a custom deleter used to help hide the impl class. Thus
  // it must use a raw delete.
  // NOLINTNEXTLINE (cppcoreguidelines-owning-memory)
  delete serverImpl;
}


/////////////////////////////////////////////////////////////////////////////
// Core Server
/////////////////////////////////////////////////////////////////////////////

void
Server::update() {
  impl->ioContext.poll();
}


std::deque<Message>
Server::receive() {
  //TODO: Update this function to call the parser's wrapper.
  // if(isNewGame)
  //TODO: once the incoming game is parsed, 
  std::deque<Message> oldIncoming;
  std::swap(oldIncoming, impl->incoming);
  return oldIncoming;
}


void
Server::send(const std::deque<Message>& messages) {
  for (const auto& message : messages) {
    auto found = impl->channels.find(message.connection);
    if (impl->channels.end() != found) {
      found->second->send(message.text);
    }
  }
}


void
Server::disconnect(Connection connection) {
  auto found = impl->channels.find(connection);
  if (impl->channels.end() != found) {
    connectionHandler->handleDisconnect(connection);
    found->second->disconnect();
    impl->channels.erase(found);
  }
}


std::unique_ptr<ServerImpl,ServerImplDeleter>
Server::buildImpl(Server& server,
                  unsigned short port,
                  std::string httpMessage) {
  // NOTE: We are using a custom deleter here so that the impl class can be
  // hidden within the source file rather than exposed in the header. Using
  // a custom deleter means that we need to use a raw `new` rather than using
  // `std::make_unique`.
  auto* impl = new ServerImpl(server, port, std::move(httpMessage));
  return std::unique_ptr<ServerImpl,ServerImplDeleter>(impl);
}

#endif
