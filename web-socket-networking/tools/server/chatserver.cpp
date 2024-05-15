/////////////////////////////////////////////////////////////////////////////
//                         Single Threaded Networking
//
// This file is distributed under the MIT License. See the LICENSE file
// for details.
/////////////////////////////////////////////////////////////////////////////

#include "Server.h"
#include "GameOrchestrator.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>
#include <unordered_set>



using networking::Connection;
using networking::Message;
using networking::Server;

std::vector<Connection> clients; //all connected users, in games, lobbies, or otherwise.

void onConnect(Connection c)
{
  std::cout << "New connection found: " << c.id << "\n";
  // TODO: add business logic for registering new users!
  clients.push_back(c);
}

void onDisconnect(Connection c)
{
  std::cout << "Connection lost: " << c.id << "\n";
  // TODO: add business logic for handling when users disconnect!
  auto eraseBegin = std::remove(std::begin(clients), std::end(clients), c);
  clients.erase(eraseBegin, std::end(clients));
}

//ServerState is a rare Global, for metadata variables useful to multiple games at once.
struct ServerState
{
  bool shouldShutdownAllGames = false; //set to true to shutdown the entire server. Considering refatoring this to an event listener instead..
};

struct LobbyMessageResult
{
  std::string result;
  bool startGame;
  bool shouldShutdown;
};

struct MessageResult
{
  std::string result;
  bool shouldShutdown;
};

struct ServerAction
{
  std::string serverOutput = "";
  std::string clientOutput = ""; // if empty, don't send anything to clients yet.
};

std::deque<Message>
buildOutgoing(const std::string &log)
{
	//TODO: rewrite this or make a copy that only sends to clients in the current game as a game.h function.
  std::deque<Message> outgoing;
  for (auto client : clients)
  {
    outgoing.push_back({client, log});
  }
  return outgoing;
}

//The main logic loop of our server:

int main(int argc, char *argv[])
{
  if (argc < 2) {
    std::cerr << "Usage:\n  " << argv[0] << " <port> \n"
              << "  e.g. " << argv[0] << " 4002\n";
    return 1;
  }
  //TODO: refactor all the inline couts below to a new ServerOutput class which will let us override them in the future.
  //e.g. ServerOutput::printInitialMessage(){
  // std::cout << "Server started. To connect clients do either of the following.\n\n";
  //}


  std::cout << "Server started. To connect clients do either of the following.\n\n";
  std::cout << "1. Open a new terminal and from the build directory run the following command \n"  
            << "  bin/client-ncurses localhost " << argv[1] << " \n\n";
  std::cout << "2. Open the following URL to open brower based interface \n"  
            << "  http://localhost:" << argv[1] << "\n\n";
  std::cout << "Listening... \n\n";


  const unsigned short port = std::stoi(argv[1]);
  Server server(port, "/", onConnect, onDisconnect);

  Games gameManager = Games(server);
  gameManager.mainServerLoop(&server, clients);

  return 0;
}
