#include <cpp-tree-sitter.h>
#include <string>
#include <string_view>
#include <iostream>

class PrintHelper {
public:
    static void printNamedTree(const ts::Node& node, int depth = 0);
    static void printTree(const ts::Node& node, int depth = 0);
    static void printTreeBold(const ts::Node& node, int depth = 0);
    static void printFieldInfo(const ts::Node& node, const std::string_view& sourceText);
    static void printConfigurationInfo(const ts::Node& root, const std::string_view& sourcecode);
    static void printConstants(const ts::Node& root, const std::string_view& sourcecode);
    static void printVariables(const ts::Node& root, const std::string_view& sourcecode);
    static void printPerPlayer(const ts::Node& root, const std::string_view& sourcecode);
    static void printPerAudience(const ts::Node& root, const std::string_view& sourcecode);
    static void printRules(const ts::Node& root, const std::string_view& sourcecode);
    static void printChildrenInfo(const ts::Node& node, const std::string_view& sourcecode);
};