#include "Setup.h"

Setup::Setup(const ts::Node& node, std::string_view gameText) : setupRoot(node), sourcecode(gameText) {
    extractSetupData();
}

void Setup::extractSetupData() {
//        Helper::printFieldInfo(setupRoot, sourcecode);
//        Helper::printChildrenInfo(setupRoot, sourcecode);
    for(uint32_t i=0; i < setupRoot.getNumChildren(); i++) {
        ts::Node child = setupRoot.getChild(i);
        if(child.getType() == "identifier") {
            extractId(child);
        } else if(child.getType() == "kind:") {
            extractKind(child);
        } else if(child.getType() == "prompt:") {
            extractPrompt(child);
        } else if(child.getType() == "default:") {
            extractDefaultValue(child);
        }
    }
    extractOptions();
}

void Setup::extractId(const ts::Node& id_node) {
    this->id = Helper::extractFieldValue(id_node, sourcecode);
}

void Setup::extractKind(const ts::Node& kind_node) {
    this->kind = Helper::extractFieldValue(kind_node.getNextSibling(), sourcecode);
}

void Setup::extractPrompt(const ts::Node& prompt_node) {
    this->prompt = Helper::extractFieldValue(prompt_node.getNextSibling(), sourcecode);
}

void Setup::extractOptions() {
    if(kind == KIND_INTEGER) {
        ts::Node numberRangeNode = getChildOfType(setupRoot, "number_range");
        appendAllOptions_numRange(numberRangeNode);
    } else if (kind == KIND_ENUM) {
        ts::Node choiceNode = getChildOfType(setupRoot, "choices:");
        appendAllOptions_choices(choiceNode);
    }
}

void Setup::extractDefaultValue(const ts::Node& default_node) {
    this->default_value = Helper::extractFieldValue(default_node.getNextSibling(), sourcecode);
}

std::string_view Setup::getId() const {
    return id;
}

std::string_view Setup::getKind() const {
    return kind;
}

std::string_view Setup::getPrompt() const {
    return prompt;
}

std::vector<std::string_view> Setup::getOptions() const {
    return options;
}
    
std::string_view Setup::getDefaultValue() const {
    return default_value;
}
    
std::string_view Setup::getValue() const {
    return input_value;
}

std::string Setup::toString() const {
    std::string result = "\nSetup data:\n";
    result.append("id: " + std::string(getId()) + "\n");
    result.append("kind: " + std::string(getKind()) + "\n");
    result.append("prompt: " + std::string(getPrompt()) + "\n");
    result.append(toString_options());
    result.append("default: " + std::string(getDefaultValue()) + "\n");
    return result;
}

std::string Setup::toString_options() const {
    std::string result = "options:\n";

    for(long unsigned int i=0; i < options.size(); i++) {
        result.append(std::string(options[i]) + "\n");
    }

    for(auto mapEntry : optionMap) {
        result.append(mapEntry.first); 
        result.append(" -> "); 
        result.append(mapEntry.second); 
        result.append("\n");
    }
    return result;
}

ts::Node Setup::getChildOfType(const ts::Node& node, const std::string_view& typeName) {
    for(uint32_t i=0; i < node.getNumChildren(); i++) {
        ts::Node child = node.getChild(i);
        if(child.getType() == typeName) {
            return child;
        }
    }
    throw ExceptionNodeWithSpecifiedTypeNotFound();
}

void Setup::appendAllOptions_numRange(const ts::Node& numRangeNode) {
    for(uint32_t i=0; i < numRangeNode.getNumChildren(); i++) {
        ts::Node child = numRangeNode.getChild(i);
        if(child.getType() == "number") {
            options.push_back(Helper::extractFieldValue(child, sourcecode));
        }
    }
}

void Setup::appendAllOptions_choices(const ts::Node& choiceNode) {
    ts::Node optionMap = choiceNode.getNextSibling().getNextSibling();
    std::string_view optionMapString = Helper::extractFieldValue(optionMap, sourcecode); 
    std::string input(optionMapString); 
    
    std::stringstream iss(input);
    std::vector<std::string> tokens;
    std::string token;

    while (std::getline(iss, token, '\n')) {
        tokens.push_back(token);
    }

    makeOptionMap(tokens); 
}

void Setup::makeOptionMap(const std::vector<std::string>& optionsList) {
    for(const std::string& optionPair : optionsList) {
        std::vector<std::string> optionValues; 
        std::stringstream ss(optionPair); 
        std::string optionValue; 

        while(std::getline(ss, optionValue, ':')) {
            optionValues.push_back(optionValue); 
        }

        std::string key = Helper::removeWrappingQuotes(Helper::trimLeadingTrailingWhitespace(optionValues[0])); 
        std::string value = Helper::removeWrappingQuotes(Helper::trimLeadingTrailingWhitespace(optionValues[1])); 
        optionMap[key] = value; 
    }
}