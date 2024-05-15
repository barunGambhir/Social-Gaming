#include "GameOrchestrator.h"
#include "Server.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include <cstdio>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <cstring>

#include <cpp-tree-sitter.h>

#include <nlohmann/json.hpp>

using networking::Connection;
using networking::Server;
using networking::Message;

extern "C" {
TSLanguage* tree_sitter_json();
TSLanguage* tree_sitter_socialgaming();
}

//Helper function to read a file and return its text as a string
std::string readFile(const std::string& filename) {
    // Open input file stream for file
    std::ifstream file = std::ifstream(filename);
    if (!file.is_open()) {
        //if file can't be opened, throw error
        std::cerr << "Error: Couldn't open file " << filename << std::endl;
        return "";
    }
    // Otherwise, read the file into a string
    std::string fileText;
    std::string line;
    //get the file line by line, adding newlines at end of each.
    while (std::getline(file, line)) {
        fileText += line + "\n"; 
    }
    file.close();

    return fileText;
}

GameOrchestrator::GameOrchestrator(std::string_view id, const ts::Node& node, const std::string_view& gameText) : id{id}, gamelogic{node, gameText} {
  //TODO implement assignment operator in gamelogic to provide a seam here for testing network without parser in future. e.g:
  // {a,b} = parseGame();
  // gamelogic{a,b};
}

void GameOrchestrator::run() {
    // gamelogic.run();
    //assume the game can run, when that function returns, the game has ended
}

void GameOrchestrator::addPlayer(const std::string_view uuid, const std::string_view name = "Anonymous") {
    Player newPlayer(uuid, name);
    activePlayers.push_back(newPlayer);
}

void GameOrchestrator::removePlayer(std::string_view uuid) {
    auto removePlayerIterator = std::remove_if(activePlayers.begin(), activePlayers.end(),
        [&uuid](const Player& currentPlayer) {
            return currentPlayer.getId() == uuid;
        });
    activePlayers.erase(removePlayerIterator, activePlayers.end());
}

std::string_view GameOrchestrator::getId() {
    return id;
}

//Given a message to create a game, make a new game and add it to the list of lobbyGames
//This is extensible to take a .game file as input, as creating a game will parse its rules at runtime.
//For now, it is hardcoded to work for rock-paper-scissors
GameOrchestrator* Games::createGame(std::string_view gameId) {
    // Create a language and parser.
    ts::Language language = tree_sitter_socialgaming();
    ts::Parser parser{language};
    //Get the rules from a .game file and store in a string
    std::string sourcecode = readFile("rockpaperscissors.game");
    if(sourcecode != ""){
      // Parse the string into a syntax tree:
      ts::Tree tree  = parser.parseString(sourcecode);
      // Get the root node of the syntax tree for creating our underlying GameLogic
      ts::Node root = tree.getRootNode();
      std::unique_ptr<GameOrchestrator> gameNetwork = std::make_unique<GameOrchestrator>(gameId, root, sourcecode);
      this->lobbyGames.push_back(std::move(gameNetwork));
      return this->lobbyGames.back().get();
    }
    return nullptr;
}

struct MessageResult
{
  std::string result;
  bool shouldShutdown;
};

//-------------------------------Client-invoked actions ---------------------------
//joinAGame, createAGame, startAGame, quit, shutdown
void quit(std::ostringstream& result, const nlohmann::json& clientMessageJSON, Games* games) {
  // TODO: handle QUIT action from client. Remove that player from the game.
  // needs a twin function, see void join and Games::attemptJoin for inspiration
  std::cout << "TODO: implement QUIT" << std::endl;
}

void shutdown(std::ostringstream& result, const nlohmann::json& clientMessageJSON, Games* games) {
  // TODO: handle SHUTDOWN action from client. End the game.
  // needs a twin function, see void join and Games::attemptJoin for inspiration
  std::cout << "TODO: implement SHUTDOWN" << std::endl;
}


void endAGame(std::ostringstream& result, const nlohmann::json& clientMessageJSON, Games* games){
//remove the game from the lobby of games
//send to the client (game owner) the message to return to the home page
//send to the game players the message of game end
  std::cout<<"Ending the game session"<<std::endl;
  std::string gameId = clientMessageJSON["gameCode"].get<std::string>();

  games->endGame(gameId);

  result << "Game is now over";

  //TODO: after clicking start a game, hide the button.
  //Rename cancel button to exit button
  // separate the cancel and game end functionalities
}

void startAGame(std::ostringstream& result, const nlohmann::json& clientMessageJSON, Games* games) {
  std::string gameId = clientMessageJSON["gameCode"].get<std::string>();

  auto foundGame = std::find_if(games->lobbyGames.begin(), games->lobbyGames.end(), [&gameId](const auto& game) {
      return game->getId() == gameId;
  });

  // if game with this id exists and hasnt started, start this game.
  if (foundGame != games->lobbyGames.end()) {
    result << "GAME IS NOW STARTING\nPlayers be ready on your devices\n\n";
    //This works as a stopgap because none of our games ever pause for input, so we dont need the activeGames vector.
    //*foundGame->run();
    games->startGame(foundGame->get());
    endAGame(result, clientMessageJSON, games);
  }
}


void joinAGame(std::ostringstream& result, const nlohmann::json& clientMessageJSON, Games* games) {
      std::string gameId = clientMessageJSON["gameCode"].get<std::string>();
      std::string playerName = clientMessageJSON["playerName"].get<std::string>();
      //add the response text from the join attempt to the result we return to players
      result << games->attemptJoin(gameId, playerName);
}

std::string Games::attemptJoin(std::string_view gameId, std::string_view playerName) {
    std::vector<std::unique_ptr<GameOrchestrator>>& lobbyGames = this->lobbyGames;

    auto gameIter = std::find_if(lobbyGames.begin(), lobbyGames.end(),
        [&gameId](const auto& game) {
            return game->getId() == gameId;
        });

    // if game with this id exists, and the player is not already in the game,
    if (gameIter != lobbyGames.end()) {
        if (!(*gameIter)->isPlayerInGame(playerName)) {
            (*gameIter)->addPlayer(playerName);
            return "Hi " + std::string(playerName) + " welcome to the game session!!\n";
        } else {
            return "Welcome back, " + std::string(playerName) + "!\n";
        }
    } else {
        return "Failed to join Game with ID " + std::string(gameId) + " is not accepting players currently. Did it already begin?\n";
    }
}

bool GameOrchestrator::isPlayerInGame(std::string_view playerName) {
    // Iterate through the players vector
    for (const auto& player : this->getActivePlayers() ) {
        // Check if the player's name matches
        if (player.getName() == playerName) {
            return true; // Player is in the game
        }
    }
    return false; // Player is not in the game
}

//invoked when a player sends a CREATE_GAME message to the server
void createAGame(std::ostringstream& result, const nlohmann::json& clientMessageJSON, Games* games) {
  std::string_view gameId = clientMessageJSON["gameCode"].get<std::string_view>();
  GameOrchestrator* newGame = games->createGame(gameId); 
  if (newGame){
    result << "Welcome to this new game session with id " << newGame->getId() << "!!\n";
  }
  else{
    result << "ERROR: unable to create new game. Is your .game file included in the build folder?";
  }
}

void cancelAGame(std::ostringstream& result, const nlohmann::json& clientMessageJSON, Games* games){
  result << "CANCEL_GAME";
}

// map of actions based on the client messages.
std::unordered_map<std::string, std::function<void(std::ostringstream&, const nlohmann::json&, Games*)>> JSONActions = {
  {"QUIT", static_cast<void(*)(std::ostringstream&, const nlohmann::json&, Games*)>(quit)},
  {"SHUTDOWN", static_cast<void(*)(std::ostringstream&, const nlohmann::json&, Games*)>(shutdown)},
  {"JOIN_GAME", static_cast<void(*)(std::ostringstream&, const nlohmann::json&, Games*)>(joinAGame)},
  {"CREATE_GAME", static_cast<void(*)(std::ostringstream&, const nlohmann::json&, Games*)>(createAGame)},
  {"START_GAME", static_cast<void(*)(std::ostringstream&, const nlohmann::json&, Games*)>(startAGame)},
  {"END_GAME", static_cast<void(*)(std::ostringstream&, const nlohmann::json&, Games*)>(endAGame)},
  {"CANCEL_GAME", static_cast<void(*)(std::ostringstream&, const nlohmann::json&, Games*)>(cancelAGame)},
};


// Uses JSON parsing to handle the messages and act accordingly
// The JSON message should contain the player id, to handle specific player actions as well
// as the game id for the game
MessageResult
processNewGameMessages(Server* server, const std::deque<Message> &incoming, Games* games)
{
  nlohmann::json returnMessage;
  std::ostringstream result;
  bool quit = false;
  for (const auto &message : incoming)
  {
    //TODO: might need to implement a timeout so if we get 1000 messages it doesnt try to parse all of them before processing the next game.
    try {
      nlohmann::json messageInJSON = nlohmann::json::parse(message.text);

      auto messageAction = messageInJSON["action"];
      //If we have an implementation for how to process this action, handle it.
      if (JSONActions.find(messageAction) != JSONActions.end()){
        JSONActions[messageAction](result, messageInJSON, games);

        returnMessage["gameCode"] = messageInJSON["gameCode"];
        returnMessage["serverMessage"] = result.str();
      } else {
        std::cout << "Action -- " << messageAction << " -- Is currently not supported\n";
      }

    } catch (const nlohmann::json_abi_v3_11_2::detail::parse_error& e) {
      std::cout << "JSON parsing error" << e.what() << std::endl;
    }
  }
  return MessageResult{returnMessage.dump(), quit};
}

std::deque<Message>
buildOutgoing(const std::string &log, std::vector<Connection> &clients)
{
	//TODO: rewrite this or make a copy that only sends to clients in the current game as a game.h function.
  std::deque<Message> outgoing;
  for (auto client : clients)
  {
    outgoing.push_back({client, log});
  }
  return outgoing;
}
struct NewGameRequest{
    std::string_view gameName;
};


// -------------------------------- Gameplay loop ------------------------------------------------  
GameOrchestrator* Games::startGame(GameOrchestrator* game) {
    game->run();
    return game;
}

//endgame functions
void Games::endGame(std::string gameId)
{
  auto foundGame = std::find_if(lobbyGames.begin(), lobbyGames.end(), [&gameId](const auto& game) {
      return game->getId() == gameId;
  });

  if(foundGame != lobbyGames.end()){
    //remove it from lobby games
    lobbyGames.erase(foundGame);
    return;
  }

  foundGame = std::find_if(activeGames.begin(), activeGames.end(), [&gameId](const auto& game) {
      return game->getId() == gameId;
  });

  if(foundGame != activeGames.end()){
    //remove it from active games 
    activeGames.erase(foundGame);
  }

}

//runOneLoopOfEachActiveGame() loops through all active games, playing a single loop of their rules. Shares the single thread of processing.
void
Games::runOneLoopOfEachActiveGame() {
  //TODO: have an iterator per game tracking where in the rules we stopped to wait last time.
  //TODO: have a timeout per game in case it hangs.
  for (std::unique_ptr<GameOrchestrator>& game : activeGames) {
      try {
          //game->run();
          //For now, we assume returning from run() means the game is over.
      } catch (const std::exception& e) {
          std::cerr << "Exception in game run: " << e.what() << std::endl;
      }
  }
  //TODO: If any games need to shutdown, update the Game's state.
}

bool GameOrchestrator::isAllPlayerResponsesRecieved()
{
  //This function only works if every time a client sends a message, we add them to respondedUsers.
  bool allConnectedClientsAnswered = this->activePlayers.size() <= this->respondedUsers.size();
  if(allConnectedClientsAnswered){
    respondedUsers.clear();
  }
  return allConnectedClientsAnswered;
}//check if all users responded by checking if length of respondents set is same length (or bigger) as length of total connected user


//mainServerLoop: ensures multiple games can run at the same time, that incoming messages are handled. 
// This function never exits, normally.
// end the server using ctrl+C from server terminal.
void Games::mainServerLoop(Server* server, std::vector<Connection> &clients){
    //server has no break condition as it should always be running
    while(true){
        server->update();
        //all actions for Games in lobby state are triggered by players, so no need to handle them here. e.g. join, start.
        runOneLoopOfEachActiveGame();
        const auto incoming = server->receive();

        //handle all client inputs, like player join requests:
        const auto [log, shouldQuit] = processNewGameMessages(server, incoming, this);

        //build outgoing messages to players.
        const auto outgoing = buildOutgoing(log, clients);
        server->send(outgoing);
    }
}