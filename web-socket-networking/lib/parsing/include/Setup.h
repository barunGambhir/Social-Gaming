#pragma once

#include <cpp-tree-sitter.h>
#include <string>
#include <string_view>
#include <vector>
#include <sstream>
#include <unordered_map>

#include "Helper.h"

class Setup {
public:
    static constexpr std::string_view KIND_INTEGER {"integer"};
    static constexpr std::string_view KIND_ENUM {"enum"};
private:
    std::string_view input_value;
    std::string_view id;
    std::string_view kind;
    std::string_view prompt;
    std::vector<std::string_view> options;
    std::unordered_map<std::string, std::string> optionMap;
    std::string_view default_value;
    ts::Node setupRoot;
    std::string_view sourcecode;
public:
    Setup(const ts::Node& node, std::string_view gameText);

    void extractSetupData();

    void extractId(const ts::Node& id_node);

    void extractKind(const ts::Node& kind_node);

    void extractPrompt(const ts::Node& prompt_node);

    void extractOptions();

    void extractDefaultValue(const ts::Node& default_node);

    std::string_view getId() const;

    std::string_view getKind() const;

    std::string_view getPrompt() const;

    std::vector<std::string_view> getOptions() const;

    std::string_view getDefaultValue() const;

    std::string_view getValue() const;

    std::string toString() const;

    std::string toString_options() const;

private:
    ts::Node getChildOfType(const ts::Node& node, const std::string_view& typeName);

    void appendAllOptions_numRange(const ts::Node& numRangeNode);

    void appendAllOptions_choices(const ts::Node& choiceNode);

    void makeOptionMap(const std::vector<std::string>& optionsList); 
};