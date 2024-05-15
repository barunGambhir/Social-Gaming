#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "Server.h"
#include "GameOrchestrator.h"

#include <thread>

using namespace testing;
using namespace networking;

class ServerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Perform any setup if needed before each test
    }

    void TearDown() override {
        // Perform any cleanup if needed after each test
    }
};

//TODO: Create server once and have access to it for each class.
class GamesTest : public ::testing::Test {
protected:
    void SetUp() override {
      //Note: don't share a server between tests! It causes bind issues with port 8000.
    }

    void TearDown() override {
    }
};

// Test the constructor
TEST_F(ServerTest, ConstructorTest) {
  auto onConnect = [](Connection c) {};
  auto onDisconnect = [](Connection c) {};

  Server server(8000, "Hello World", onConnect, onDisconnect);

  // Would do something like this for an assertion
  //EXPECT_EQ(server.getPort(), 8000);
}

// Test the update functionality
TEST_F(ServerTest, UpdateTest) {
  auto onConnect = [](Connection c) {};
  auto onDisconnect = [](Connection c) {};

  Server server(8000, "Hello World", onConnect, onDisconnect);

  // Perform update
  server.update();
}

// Test the send functionality
TEST_F(ServerTest, SendTest) {
  auto onConnect = [](Connection c) {};
  auto onDisconnect = [](Connection c) {};

  Server server(8000, "Hello World", onConnect, onDisconnect);

  // Populate test messages
  std::deque<networking::Message> messages = {
    { {1}, "Message 1" },
    { {2}, "Message 2" },
    // Add more messages as needed
  };

  server.send(messages);

  // Would do something like this for an assertion
  //EXPECT_TRUE(server.messagesSentSuccessfully());
}

// Test the receive functionality
TEST_F(ServerTest, ReceiveTest) {
  auto onConnect = [](Connection c) {};
  auto onDisconnect = [](Connection c) {};

  networking::Server server(8000, "Hello World", onConnect, onDisconnect);

  std::deque<networking::Message> receivedMessages = server.receive();

  EXPECT_TRUE(receivedMessages.empty());
}


TEST_F(ServerTest, DisconnectTest) {
  auto onConnect = [](Connection c) {};
  auto onDisconnect = [](Connection c) {};

  // Assuming port is 8080 and httpMessage is "Hello World"
  Server server(8080, "Hello World", onConnect, onDisconnect);

  // Perform a disconnect
  Connection connectionToDisconnect = {8080}; 
  server.disconnect(connectionToDisconnect);

  // Would do something like this for an assertion
  // EXPECT_TRUE(server.disconnectSucceeded());
}

// --------------Testing the Game Class --------------------------------------------------------

// Define a mock Game class to replace the actual Game class
class MockGame : public VirtualGame {
public:
    //MOCK_METHOD0(run, void());
    MOCK_METHOD(void, run, (), (override));
};

//trivially simple test to ensure I have mocking syntax correct. and hey, more tests never hurts.
TEST_F(GamesTest, RunMethod) {
    MockGame mockGame;
    EXPECT_CALL(mockGame, run()).Times(1);
    mockGame.run();
}

//Note, we changed activeGames to be unique pointers to GameOrchestrators instead of to virtualGames
//This removed the seam along which mocking classes could easily be added.
//Thus, this test is no longer useful in its current form, as we can no longer mock the games easily.

//Confirm game's .run() function is called correctly by runOneLoopOfEachActiveGame
// TEST_F(GamesTest, runOneLoopOfEachActiveGame) {
//     // Create a mock Game object that returns immediately when run() is called
//     MockGame mockGame1;
//     MockGame mockGame2;

//     // Expect run() to be called once on every game.
//     EXPECT_CALL(mockGame1, run()).Times(1);
//     EXPECT_CALL(mockGame2, run()).Times(1);
//     ON_CALL(mockGame1, run()).WillByDefault(Return());
//     ON_CALL(mockGame2, run()).WillByDefault(Return());

//     // Create a vector of 2 mocked Game pointers
//     std::vector<VirtualGame*> games;
//     games.push_back(&mockGame1);
//     games.push_back(&mockGame2);

//     auto onConnect = [](Connection c) {};
//     auto onDisconnect = [](Connection c) {};
//     Server server(8000, "Hello World", onConnect, onDisconnect);

//     Games gamesManager = Games(server);
//     createGame(std::string_view newGameId);

    
//     gamesManager.runOneLoopOfEachActiveGame();
// }


//TODO: confirm runOneLoopOfEachActiveGame exits gracefully when run() throws an exception

//TODO:confirm runOneLoopOfEachActiveGame exits gracefully after 10 seconds when run() stalls

//Confirm GamesHandler can create a new a game
TEST_F(GamesTest, AddingGameToGames) {
    auto onConnect = [](Connection c) {};
    auto onDisconnect = [](Connection c) {};
    Server server(8000, "Hello World", onConnect, onDisconnect);//TODO maybe mock the server so we dont collide?
    Games gamesManager = Games(server);
    std::string_view gameId = "ABCD1234";
    GameOrchestrator* newGame = gamesManager.createGame(gameId);
    //creating a new game 
    ASSERT_TRUE(newGame != nullptr) << "createGame() isn't returning a valid pointer to the new Game Object";

    // Add assertions to check the length of lobbyGames and activeGames
    ASSERT_EQ(gamesManager.lobbyGames.size(), 1) << "After createGame(), lobbyGames length should be 1";
    ASSERT_EQ(gamesManager.activeGames.size(), 0) << "activeGames length should be 0";
}

//Confirm GamesHandler can create a new a game, then start it.
TEST_F(GamesTest, StartingAGameInGames) {
    auto onConnect = [](Connection c) {};
    auto onDisconnect = [](Connection c) {};
    Server server = Server(8000, "Hello World", onConnect, onDisconnect);

    Games gamesManager = Games(server);
    //Game newGame(id=1);
    std::string_view gameId = "ABCD1234";
    GameOrchestrator* newGame = gamesManager.createGame(gameId);
    //actually start the game, update pointer once the underlying game moves vectors
    newGame = gamesManager.startGame(newGame);

    // Add assertions to check the length of lobbyGames and activeGames
    ASSERT_EQ(gamesManager.lobbyGames.size(), 0) << "After startGame(), lobbyGames length should be 0";
    ASSERT_EQ(gamesManager.activeGames.size(), 1) << "activeGames length should be 1";
}

TEST_F(GamesTest, EndingGames) {
    auto onConnect = [](Connection c) {};
    auto onDisconnect = [](Connection c) {};
    Server server = Server(8000, "Hello World", onConnect, onDisconnect);
    Games gamesManager = Games(server);
    //std::string_view gameId = "ABCD1234";
    //GameOrchestrator* newGame = gamesManager.createGame(gameId);

    //gamesManager.end_game(); //TODO implement an end_game() function

    ASSERT_EQ(gamesManager.lobbyGames.size(), 0) << "After createGame(), lobbyGames length should be 0";
    ASSERT_EQ(gamesManager.activeGames.size(), 0) << "activeGames length should be 0";
}

//TODO: Add test that GamesManager can run runLobbyLoopOnce(Server* server) when it has a Game created but not started

/*Testing the code generation and URL redirection functions in server.cpp*/

TEST_F(GamesTest, ValidCodes) {
  // generate a random code and check its validity
  std::string newCode = generateRandomCode();
  auto it = std::find_if(newCode.begin(), newCode.end(), [](char const &c) {
            return !isalnum(c);
        });
  EXPECT_TRUE(it==newCode.end());
  EXPECT_TRUE(newCode.size() == 8);
}

TEST_F(GamesTest, InvalidCodes) {
  // Test invalid codes (not length 8 or containing non-alphanumeric characters)
  // Length less than 8
  EXPECT_FALSE(isValidGameCode("INVALID"));
  // Length less than 8 
  EXPECT_FALSE(isValidGameCode("INVALID12"));
  // Length more than 8 
  EXPECT_FALSE(isValidGameCode("123456789"));
  // Contains non-alphanumeric character '@'
  EXPECT_FALSE(isValidGameCode("INVALID@1"));
  // Contains multiple non-alphanumeric characters 
  EXPECT_FALSE(isValidGameCode("I*(%^|D1")); 
}

TEST(GameTest, ExtractFirstValueTest) {

  // default empty URL string
  std::string_view url = "";
  std::string expected = "landingpage";
  ASSERT_EQ(extractFirstValueFromRequest(url), expected);

  //no slashes with text
  url = "example";
  expected = "landingpage";
  ASSERT_EQ(extractFirstValueFromRequest(url), expected);

  //Passing an empty URL with a slash
  url = "/";
  expected = "landingpage";
  ASSERT_EQ(extractFirstValueFromRequest(url), expected);

  //including only one slash ('/') in the input
  url = "/test";
  expected = "test";
  ASSERT_EQ(extractFirstValueFromRequest(url), expected);

  //including two slashes 
  url = "/test1/test2";
  expected = "test1";
  ASSERT_EQ(extractFirstValueFromRequest(url), expected);

  //including multiple slashes
  url = "/example/another/value";
  expected = "example";
  ASSERT_EQ(extractFirstValueFromRequest(url), expected);

}

TEST(GameTest, AddToPlayerUrlTest){

  //Empty URL
  std::string_view url = "";
  std::string expected = "/joinroom/invalidcode";
  EXPECT_EQ(addToPlayerURL(url), expected);

  //Invalid game code
  url = "https://example.com?game=456&player=InvalidCode";
  //expected = "/joinroom/invalidcode";
  EXPECT_EQ(addToPlayerURL(url), expected);

  //no player name provided
  url = "https://example.com?game=789";
  //expected = "/joinroom/invalidcode";
  EXPECT_EQ(addToPlayerURL(url), expected);

  //no game code provided
  url = "https://example.com?player=JaneDoe";
  //expected = "/joinroom/invalidcode"; 
  EXPECT_EQ(addToPlayerURL(url), expected);

  //invalid URL format
  url = "invalid_url_format";
  //expected = "/joinroom/invalidcode"; 
  EXPECT_EQ(addToPlayerURL(url), expected);

  //TODO: add a case for a valid code

}
//TODO: add test that sending invalid json to server is handled gracefully
//TODO: confirm trying to add a player to a game she's already in fails gracefully
//TODO: confirm a game cannot start with 0 players
//TODO: confirm a play attempting to join a game can do so, given valid json and an existing game.

//TODO: write more test ideas here.
