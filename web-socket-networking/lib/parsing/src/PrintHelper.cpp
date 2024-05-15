#include "PrintHelper.h"

void PrintHelper::printNamedTree(const ts::Node& node, int depth) {
    for (int i = 0; i < depth; ++i) {
        std::cout << "  ";
    }

    if (node.isNamed()) {
        std::cout << std::string(node.getType().data(), node.getType().size()) << std::endl;
    }

    for (uint32_t i = 0; i < node.getNumNamedChildren(); ++i) {
        printNamedTree(node.getNamedChild(i), depth + 1);
    }
}

void PrintHelper::printTree(const ts::Node& node, int depth) {
    for (int i = 0; i < depth; ++i) {
        std::cout << "  ";
    }

    std::cout << std::string(node.getType().data(), node.getType().size()) << std::endl;

    for (uint32_t i = 0; i < node.getNumChildren(); ++i) {
        printTree(node.getChild(i), depth + 1);
    }
}

void PrintHelper::printTreeBold(const ts::Node& node, int depth) {
    // ANSI escape code for bold text
    const char* boldStart = "\033[1m";
    const char* boldEnd = "\033[0m";

    std::string indentation(depth, ' ');

    for (uint32_t i = 0; i < node.getNumChildren(); ++i) {
        ts::Node childNode = node.getChild(i);

        if (childNode.isNamed()) {
        std::cout << indentation << boldStart << childNode.getType() << boldEnd << std::endl;
        } else {
        std::cout << indentation << childNode.getType() << std::endl;
        }

        printTreeBold(childNode, depth + 1);
    }
}

void PrintHelper::printFieldInfo(const ts::Node& node, const std::string_view& sourceText) {
    if (node.isNamed()) {
        std::string_view fieldType = node.getType();
        auto byteRange = node.getByteRange();

        // Extract the field value from the source text using the byte range
        std::string fieldValue(sourceText.substr(byteRange.start, byteRange.end - byteRange.start));

        std::cout << "Field Type: " << fieldType << ", Field Value: " << fieldValue << "\n";
    }
}

void PrintHelper::printConfigurationInfo(const ts::Node& root, const std::string_view& sourcecode) {
    ts::Node configuration = root.getChildByFieldName("configuration");
    ts::Node name = configuration.getChildByFieldName("name");
    ts::Node player_range = configuration.getChildByFieldName("player_range");
    ts::Node audience = configuration.getChildByFieldName("has_audience");
    ts::Node setup = configuration.getChildByFieldName("setup_rule");

    // setup isn't working, i think it has something to do with getting the name of the rule
    if (setup.isNull()) {
        std::cout << "Setup is null" << std::endl;
    }

    printFieldInfo(name, sourcecode);
    printFieldInfo(player_range, sourcecode);
    printFieldInfo(audience, sourcecode);
    //printFieldInfo(setup, sourcecode); // setup not working; if found please fix
}

void PrintHelper::printConstants(const ts::Node& root, const std::string_view& sourcecode) {
    ts::Node constants = root.getChildByFieldName("constants");
    printFieldInfo(constants, sourcecode);
}

void PrintHelper::printVariables(const ts::Node& root, const std::string_view& sourcecode) {
    ts::Node variables = root.getChildByFieldName("variables");
    ts::Node map = variables.getChildByFieldName("map");
    printFieldInfo(map,sourcecode);
}

void PrintHelper::printPerPlayer(const ts::Node& root, const std::string_view& sourcecode) {
    ts::Node per_player = root.getChildByFieldName("per_player");
    ts::Node map = per_player.getChildByFieldName("map");
    printFieldInfo(map,sourcecode);
}

void PrintHelper::printPerAudience(const ts::Node& root, const std::string_view& sourcecode) {
    ts::Node per_audience = root.getChildByFieldName("per_audience");
    ts::Node map = per_audience.getChildByFieldName("map");
    printFieldInfo(map,sourcecode);
}

void PrintHelper::printRules(const ts::Node& root, const std::string_view& sourcecode) {
    ts::Node rules = root.getChildByFieldName("rules");
    printFieldInfo(rules,sourcecode);
}

void PrintHelper::printChildrenInfo(const ts::Node& node, const std::string_view& sourcecode) {
    for(uint32_t i=0; i < node.getNumChildren(); i++) {
        ts::Node child = node.getChild(i);
        printFieldInfo(child, sourcecode);
        //printChildrenInfo(child, sourcecode);
    }
}
