#pragma once

#include <cpp-tree-sitter.h>
#include <vector>
#include "Expression.h"
#include "Environment.h"
#include "MapHandler.h"
#include "Helper.h"
#include "Setup.h"
#include "Configuration.h"
#include "Rules.h"
#include "PrintHelper.h"

class GameLogic {
private:
    Configuration configuration;
    ts::Node root;
    std::string_view sourcecode;
    MapHandler constants;
    MapHandler variables;
    MapHandler perPlayer;
    MapHandler perAudience;
    std::vector<std::unique_ptr<Expression>> topLevelExpressions;
    Environment globalEnv;

    int currentRound;
public:
    GameLogic(const ts::Node& node, const std::string_view& gameText);
    void extractGameData();
    void extractConfiguration();
    void extractConstants();
    void extractVariables();
    void extractPerPlayer();
    void extractPerAudience();
    void extractRules();
    Configuration getConfiguration() const;
    MapHandler &getConstants();
    MapHandler &getVariables();
    MapHandler &getPerPlayer();
    MapHandler &getPerAudience();
    Environment& getEnvironment();
    void run();
    void setTopLevelExpressions(std::vector<std::unique_ptr<Expression>> expList); 
    void setGlobalEnv(Environment env); 

    std::string getStateAsString(std::string_view var); 
};
