#include "Configuration.h"

Configuration::Configuration() {}

void Configuration::extractGameName(const ts::Node& configNode, const std::string_view& sourcecode) {
    ts::Node name = configNode.getChildByFieldName("name");
    gameName = Helper::extractFieldValue(name, sourcecode);
}

void Configuration::extractPlayerRange(const ts::Node& configNode, const std::string_view& sourcecode) {
    ts::Node player_range = configNode.getChildByFieldName("player_range");
    for(uint32_t i=0; i < player_range.getNumChildren(); i++) {
        ts::Node child = player_range.getChild(i);
        if(child.getType() == "number") {
            auto valueStr = Helper::covertViewToString(Helper::extractFieldValue(child, sourcecode));
            playerRange.push_back(std::stoi(valueStr));
        }
    }
}

void Configuration::extractHasAudience(const ts::Node& configNode, const std::string_view& sourcecode) {
    ts::Node audience = configNode.getChildByFieldName("has_audience");
    auto has_audience_str = Helper::extractFieldValue(audience, sourcecode);
    if(has_audience_str == "true") {
        has_audience = true;
    } else {
        has_audience = false;
    }
}

void Configuration::extractSetups(const ts::Node& node, const std::string_view& sourcecode) {
    for(uint32_t i=0; i < node.getNumChildren(); i++) {
        ts::Node child = node.getChild(i);
        if(child.getType() == "setup_rule") {
            Setup setup(child, sourcecode);
            this->setups.push_back(setup);
        }
    }
}

void Configuration::extractData(const ts::Node& configNode, const std::string_view& sourcecode) {
    extractGameName(configNode, sourcecode);
    extractPlayerRange(configNode, sourcecode);
    extractHasAudience(configNode, sourcecode);
    extractSetups(configNode, sourcecode);
}

std::string_view Configuration::getGameName() const {
    return gameName;
}

std::vector<int> Configuration::getPlayerRange() const {
    return playerRange;
}

bool Configuration::getHasAudience() const {
    return has_audience;
}

std::vector<Setup> Configuration::getSetups() const {
    return setups;
}