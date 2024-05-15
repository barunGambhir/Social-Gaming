#include "Rules.h"

std::vector<std::unique_ptr<Expression>> Rules::extractRules(const ts::Node& rulesNode, std::string_view sourcecode) {
    std::vector<std::unique_ptr<Expression>> rules;

    auto rulesBodyNode = rulesNode.getChildByFieldName("body"); 
    for(uint32_t i=0; i < rulesBodyNode.getNumChildren(); i++) {
        auto child = rulesBodyNode.getChild(i); 
        if (child.getType() == "rule") {
            auto ruleType = child.getChild(0).getType();
            if (ruleType == "message") {
                auto messageExp = extractMessageExp(child.getChild(0), sourcecode);
                rules.push_back(std::move(messageExp));
            } else if (ruleType == "discard") {
                auto discardExp = extractDiscardExp(child.getChild(0), sourcecode);
                rules.push_back(std::move(discardExp));
            } else if (ruleType == "scores") {
                auto scoresExp = extractScoresExp(child.getChild(0), sourcecode);
                rules.push_back(std::move(scoresExp));
            } else if (ruleType == "for") {
                auto forExp = extractForExp(child.getChild(0), sourcecode);
                rules.push_back(std::move(forExp));
            }
        }
    }
    return rules;
}

std::unique_ptr<Expression> Rules::extractMessageExp(const ts::Node& messageNode, std::string_view sourcecode) {
    auto targetNode = messageNode.getChildByFieldName("players"); 
    auto target = Helper::extractFieldValue(targetNode, sourcecode); 

    auto contentNode = messageNode.getChildByFieldName("content"); 
    std::string_view content = Helper::extractFieldValue(contentNode, sourcecode); 

    auto messageExp = std::make_unique<MessageExp>(target, content); 
    return messageExp; 
}

std::unique_ptr<Expression> Rules::extractDiscardExp(const ts::Node& discardNode, std::string_view sourcecode) {
    ExtractWhatItem whatToDiscard; 
    ExtractWhatItem fromWhere; 

    for(uint32_t i=0; i < discardNode.getNumChildren(); i++) {
        ts::Node child = discardNode.getChild(i);
        auto childType = child.getType(); 
        if(childType == "expression") {
            whatToDiscard = getExtractWhat(child, sourcecode); 
        } else if (childType == "qualified_identifier") {
            fromWhere = getExtractWhat(child, sourcecode); 
        }
    }

    auto result = std::make_unique<DiscardExp>(whatToDiscard, fromWhere); 
    return result; 
}

std::unique_ptr<Expression> Rules::extractScoresExp(const ts::Node& scoresNode, std::string_view sourcecode) {
    ts::Node listLiteralNode = scoresNode.getChild(0); 
    for(uint32_t i=0; i < scoresNode.getNumChildren(); i++) {
        listLiteralNode = scoresNode.getChild(i);
        auto childType = listLiteralNode.getType(); 
        if(childType == "list_literal") {
            break; 
        }
    }
    ts::Node expressionListNode = listLiteralNode.getChild(1);

    std::vector<ExtractWhatItem> items; 
    for(uint32_t i=0; i < expressionListNode.getNumChildren(); i++) {
        ts::Node child = expressionListNode.getChild(i);
        auto childType = child.getType(); 
        if(childType == "expression") {
            items.emplace_back(getExtractWhat(child, sourcecode)); 
        }
    }

    auto result = std::make_unique<ScoresExp>(items); 
    return result; 
}


std::unique_ptr<Expression> Rules::extractForExp(const ts::Node& forNode, std::string_view sourcecode) {
    std::string loopVar;
    int min;

    for(uint32_t i=0; i < forNode.getNumChildren(); i++) {
        auto child = forNode.getChild(i);
        auto childType = child.getType();
        if(childType == "identifier") {
            loopVar = Helper::extractFieldValue(child, sourcecode);
        } else if (childType == "expression") {
            min = std::stoi(getExtractWhat(child, sourcecode).getArguments()[0]);
        }
    }

    //todo: this causes program to crash:
    // auto rules = extractRules(forNode, sourcecode);

    //for now, let's just assume the round is upto 5 until we figure out name resolution:
    auto result = std::make_unique<ForExp>(loopVar, min, 5);
    return result;
}



ExtractWhatItem Rules::getExtractWhat(const ts::Node& node, std::string_view sourcecode) {
    //has 4 children of type: expression, ., builtin, argument_list
    ExtractWhatItem extractWhatItem;
    for(uint32_t i=0; i < node.getNumChildren(); i++) {
        auto child = node.getChild(i);
        auto childType = child.getType(); 
        if(childType == "expression" || childType == "identifier" || childType == "quoted_string") {
            auto value = Helper::extractFieldValue(child, sourcecode);
            extractWhatItem.setCallChain(value);
        } else if(childType == "builtin") {
            auto value = Helper::extractFieldValue(child, sourcecode);
            extractWhatItem.setBuiltin(value);
        } else if(childType == "argument_list") {
            auto value = Helper::extractFieldValue(child, sourcecode);
            extractWhatItem.setArguments(value);
        }
    }
    return extractWhatItem; 
}

