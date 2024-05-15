#pragma once

#include <cpp-tree-sitter.h>

#include "Setup.h"


class Configuration {
private:
    std::string_view gameName;
    std::vector<int> playerRange;
    bool has_audience;
    std::vector<Setup> setups;
public:
    Configuration();

    void extractGameName(const ts::Node& configNode, const std::string_view& sourcecode);

    void extractPlayerRange(const ts::Node& configNode, const std::string_view& sourcecode);

    void extractHasAudience(const ts::Node& configNode, const std::string_view& sourcecode);

    void extractSetups(const ts::Node& node, const std::string_view& sourcecode);

    void extractData(const ts::Node& configNode, const std::string_view& sourcecode);

    std::string_view getGameName() const;

    std::vector<int> getPlayerRange() const;

    bool getHasAudience() const;

    std::vector<Setup> getSetups() const;

};