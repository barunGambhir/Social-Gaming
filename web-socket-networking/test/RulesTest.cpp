#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cpp-tree-sitter.h>
#include "Rules.h"
#include "NodeForTesting.cpp"

class MockExtractWhatItem : public ExtractWhatItem {
public:
    MOCK_METHOD(void, setCallChain, (const std::string&), (override));
    MOCK_METHOD(void, setBuiltin, (const std::string&), (override));
    MOCK_METHOD(void, setArguments, (const std::string&), (override));
};

class MockRules : public Rules {
public:
    MOCK_METHOD(ExtractWhatItem, getExtractWhat, (const NodeTest&, std::string_view), (override));
};

class RulesTest : public ::testing::Test {
protected:
    MockRules rules;
};

TEST_F(RulesTest, ExtractMessageExp) {
NodeTest messageNode("message", "messageName", "buffer-message-node-value");
messageNode.addChild(NodeTest("identifier", "players", "all"));
messageNode.addChild(NodeTest("string", "content", "round {round}"));
std::string sourcecode = readFile("rockpaperscissors.game");

MockExtractWhatItem mockExtractWhatItem;
EXPECT_CALL(rules, getExtractWhat(_, _))
.WillOnce(::testing::ReturnRef(mockExtractWhatItem));

auto result = rules.extractMessageExp(messageNode, sourcecode);

std::string expected = "all" + "round 2";
EXPECT_EQ(result->getContent(), expected);
}

TEST_F(RulesTest, ExtractDiscardExp) {
NodeTest discardNode("discard", "discard-value");
discardNode.addChild(NodeTest("expression", "", "winners.size()"));
discardNode.addChild(NodeTest("qualified_identifier", "", "winners"));

std::string sourcecode = readFile("rockpaperscissors.game");

MockExtractWhatItem mockExtractWhatItem;
EXPECT_CALL(rules, getExtractWhat(_, _))
.WillOnce(::testing::ReturnRef(mockExtractWhatItem));

auto result = rules.extractDiscardExp(discardNode, sourcecode);

std::string expected = "Call chain: winners\n";
expected += "Builtin: size\n";
expected += "Arguments:\n";

EXPECT_EQ(result->getFromWhere(), expected);
}

TEST_F(RulesTest, ExtractScoresExp) {
NodeTest scoresNode("scores", "scores-value");
NodeTest expression("expression", "", "[wins]");
NodeTest list("list_literal", "", "");
list.addChild(expression);
scoresNode.addChild(list);

std::string sourcecode = readFile("rockpaperscissors.game");

MockExtractWhatItem mockExtractWhatItem;
EXPECT_CALL(rules, getExtractWhat(_, _))
.WillOnce(::testing::ReturnRef(mockExtractWhatItem));

auto result = rules.extractScoresExp(scoresNode, sourcecode);

EXPECT_EQ(result->getItems().size(), 1);
}

TEST_F(RulesTest, ExtractRules) {
NodeTest rulesNode("rules", "", "");

NodeTest messageNode("message", "messageName", "buffer-message-node-value");
messageNode.addChild(NodeTest("identifier", "players", "all"));
messageNode.addChild(NodeTest("string", "content", "round {round}"));

NodeTest discardNode("discard", "discard-value");
discardNode.addChild(NodeTest("expression", "", "winners.size()"));
discardNode.addChild(NodeTest("qualified_identifier", "", "winners"));

NodeTest scoresNode("scores", "scores-value");
NodeTest expression("expression", "", "[wins]");
NodeTest list("list_literal", "", "");
list.addChild(expression);
scoresNode.addChild(list);
std::string sourcecode = readFile("rockpaperscissors.game");

rulesNode.addChild(messageNode);
rulesNode.addChild(discardNode);
rulesNode.addChild(scoresNode);

MockExtractWhatItem mockExtractWhatItem;
EXPECT_CALL(rules, getExtractWhat(_, _))
.WillOnce(::testing::ReturnRef(mockExtractWhatItem));

auto result = rules.extractRules(rulesNode, sourcecode);

EXPECT_EQ(result.size(), 3);
}


