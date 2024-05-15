#include "GameLogic.h"

//Is this handling storing game data?

//Is this responsible for parsing game data and returning it?

GameLogic::GameLogic(const ts::Node& node, const std::string_view& gameText) : root(node), sourcecode(gameText) {
    extractGameData();
    extractConstants();
    extractVariables();
    extractPerPlayer();
    extractPerAudience();
    extractRules();
    currentRound = 5;
}

void GameLogic::extractGameData() {
    extractConfiguration();
}

void GameLogic::extractConfiguration() {
    ts::Node configNode = root.getChildByFieldName("configuration");
    configuration.extractData(configNode, sourcecode);
}

void GameLogic::extractConstants() {
    ts::Node constantsNode = root.getChildByFieldName("constants");
    constants = MapHandler(constantsNode, sourcecode);
}

void GameLogic::extractVariables() {
    ts::Node variablesNode = root.getChildByFieldName("variables");
    variables = MapHandler(variablesNode, sourcecode);
}

void GameLogic::extractRules() {
    ts::Node rulesNode = root.getChildByFieldName("rules"); 
    setTopLevelExpressions(Rules::extractRules(rulesNode, sourcecode)); 
}

void GameLogic::extractPerPlayer() {
    ts::Node perPlayerNode = root.getChildByFieldName("per_player");
    perPlayer = MapHandler(perPlayerNode, sourcecode);
}

void GameLogic::extractPerAudience() {
    ts::Node perAudienceNode = root.getChildByFieldName("per_audience");
    perAudience = MapHandler(perAudienceNode, sourcecode);
}

Configuration GameLogic::getConfiguration() const {
    return configuration;
}

MapHandler& GameLogic::getConstants()  {
    return constants;
}

MapHandler& GameLogic::getVariables()  {
    return variables;
}

MapHandler& GameLogic::getPerPlayer() {
    return perPlayer;
}

MapHandler& GameLogic::getPerAudience() {
    return perAudience;
}

void GameLogic::run() {
    for (auto const& expression : topLevelExpressions) {
        expression->evaluate(globalEnv, *this); 
        // Will add try/catch to handle SocialGaming runtime exceptions wihtout crashing the entire program.
    }
    
}

void GameLogic::setTopLevelExpressions(std::vector<std::unique_ptr<Expression>> expList) {
    topLevelExpressions = std::move(expList); 
}

void GameLogic::setGlobalEnv(Environment env) {
    globalEnv = env; 
}

std::string GameLogic::getStateAsString(std::string_view var) {
    std::string result = "{" + std::string(var) + "}"; 
    if (var == "round") {
        result = std::to_string(this->currentRound);
    }
    return result; 
}


Environment& GameLogic::getEnvironment() {
    return globalEnv;
}