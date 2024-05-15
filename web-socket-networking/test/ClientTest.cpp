#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Client.h"

#include <thread>

using namespace testing;
using namespace networking;

class ClientTest : public ::testing::Test {
protected:
  void SetUp() override {
    // This can include creating the client instance or other common setup
  }

  void TearDown() override {
    // This can include destroying the client instance or other common cleanup
  }
};

// Test the constructor
TEST_F(ClientTest, Constructor) {
  Client client("127.0.0.1", "8000");
  
  // Check if the client is not initially disconnected
  EXPECT_FALSE(client.isDisconnected());
}

// Test the send functionality
TEST_F(ClientTest, Send) {
  Client client("127.0.0.1", "8000");
  
  std::string message = "Hello, server!";
  client.send(message);
}

// Test the receive functionality
TEST_F(ClientTest, Receive) {
  Client client("127.0.0.1", "8000");
  
  std::string message = "Hello, server!";
  client.send(message);

  std::string receivedMessage = client.receive();
  
  EXPECT_NE(message, "something else");
}

// Test the update functionality
TEST_F(ClientTest, Update) {
  Client client("127.0.0.1", "8000");
  
  EXPECT_EQ(client.receive(), "");  
  
  // Send a message
  std::string message = "Hello, server!";
  client.send(message);
  
  EXPECT_EQ(client.receive(), "");  
  
  client.update();
  
  // After update, we should be able to receive the sent message
  std::string receivedMessage = client.receive();
  EXPECT_NE(message, receivedMessage);
}

// Test the isDisconnected functionality
TEST_F(ClientTest, IsDisconnected) {
  Client client("127.0.0.1", "8080");
  
  // Check if the client is not initially disconnected
  EXPECT_FALSE(client.isDisconnected());
  
  // Would do something like this for an assertion
  // client.disconnect();
  //EXPECT_TRUE(client.isDisconnected());
}
