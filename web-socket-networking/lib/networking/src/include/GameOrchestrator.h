#ifndef GAME_ORCHESTRATOR_H
#define GAME_ORCHESTRATOR_H

#include "Player.h"
#include "Expression.h"
#include "../../../parsing/include/GameLogic.h"
#include <vector>
#include <memory>

//Forward declaration
namespace networking {
    class Server; // Forward declaration for the Server class
    struct Connection;
}

//Used to provide a seam for unit tests to mock our GameOrchestrator class.
class VirtualGame {
public:
    //TODO: make this stuff copy-able. I can't figure out the unique pointer shenanigans.
    //Game(std::vector<std::unique_ptr<Expression>> exp, Environment env) : topLevelExpressions{std::move(exp)}, globalEnv{env} {};
    //Game(const Game& other); // Copy constructor
    //Game& operator=(const Game& other); // Copy assignment operator
    virtual void run() = 0;
    virtual ~VirtualGame() {}
    void addPlayer(const std::string& name, const std::string& uuid); // get id from client.connection.id
    void removePlayer(const std::string& uuid);
};
// --------------- Game Orchestrator, for coordinating timing and network interactions of individual games -----------------------------------------------------------
class GameOrchestrator : public VirtualGame {
public:
    GameOrchestrator(std::string_view id, const ts::Node& node, const std::string_view& gameText); // Constructor
    void run() override;
    void addPlayer(std::string_view name, std::string_view uuid);
    void removePlayer(std::string_view uuid);
    std::string_view getId();
    const std::vector<Player>& getActivePlayers() const {
        return activePlayers;
    }
    bool isAllPlayerResponsesRecieved();
    bool isPlayerInGame(std::string_view playerName);
private:
    const std::string id; //the unique id of the game, e.g. GFMHUDRL
    std::vector<Player> activePlayers;
    std::vector<Player> respondedUsers;
    GameLogic gamelogic;
};

// --------------- Games Handler, for controlling all games ------------------------------------------------------------
class Games{
public:
  Games(networking::Server &server): server{&server} {};
  std::vector<std::unique_ptr<GameOrchestrator>> activeGames;
  std::vector<std::unique_ptr<GameOrchestrator>> lobbyGames;

  //createGame: Given a message to create a game, make a new game and add it to the list of lobbyGames
  GameOrchestrator* createGame(std::string_view newGameId);
  
  //startGame: Given a starting game, move game from lobbyGames to ActiveGames list.
  //Takes a pointer to the game, and returns an updated pointer to the game after it has been moved.
  GameOrchestrator* startGame(GameOrchestrator*);

  /**
   *  runOneLoopOfEachActiveGame: Runs a single pass through all the logic in each active Game's Rules.
   *  This is called in a while() loop in chatserver.cpp to make the games run. 
   *  Games should only be added to ActiveGames once exiting the lobby with all players.
   *  iterates across std::vector<VirtualGame*> activeGames
   */
  void runOneLoopOfEachActiveGame();

  //This function abstracts actual printing logic away from chatserver.cpp
  void printInitialMessage();

  //The main logic loop of our server. It listens for new games to create, then runs through a loop of each active game, helping to ensure fairness.
  void mainServerLoop(networking::Server* server, std::vector<networking::Connection> &clients);

  //attemptJoin: handles attempts for new players to join existing games.
  std::string attemptJoin(std::string_view gameId, std::string_view playerName);

    //function to end a game that hasn't been started yet
  void endGame(std::string);
    
    // funciton to end an active game 

private:
  networking::Server* server;
};

#endif
