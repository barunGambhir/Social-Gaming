#pragma once

#include <cpp-tree-sitter.h>
#include <string>
#include <string_view>
#include <vector>

#include "Expression.h"
#include "Helper.h"
#include "ExtractWhatItem.h"

class Rules {
public:
    static std::vector<std::unique_ptr<Expression>> extractRules(const ts::Node& rulesNode, std::string_view sourcecode); 
private:
    static std::unique_ptr<Expression> extractMessageExp(const ts::Node& messageNode, std::string_view sourcecode); 
    static std::unique_ptr<Expression> extractDiscardExp(const ts::Node& discardNode, std::string_view sourcecode); 
    static std::unique_ptr<Expression> extractScoresExp(const ts::Node& scoresNode, std::string_view sourcecode);
    static std::unique_ptr<Expression> extractForExp(const ts::Node& forNode, std::string_view sourcecode);
    static ExtractWhatItem getExtractWhat(const ts::Node& node, std::string_view sourcecode);
};
