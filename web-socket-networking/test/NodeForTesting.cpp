#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

class NodeTest {
private:
    std::string type;
    std::string name;
    std::string value;
    std::vector<NodeTest> children;
    std::unordered_map<std::string, NodeTest> childMap;
    void updateChildMap();
public:
    //for now just store value in the node?
    //todo: need to figure out how to link the node's value into the sourcetext
    NodeTest(const std::string& type, const std::string& name, const std::string& value);
    void addChild(const NodeTest& child);
    NodeTest getChildByFieldName(const std::string& fieldName) const;
    std::string getType() const;
    void print() const;
};



NodeTest::NodeTest(const std::string& type, const std::string& name, const std::string& value)
        : type(type), name(name), value(value) {}

void NodeTest::addChild(const NodeTest& child) {
    children.push_back(child);
}

NodeTest NodeTest::getChildByFieldName(const std::string& fieldName) const {
    auto it = childMap.find(fieldName);
    return (it != childMap.end()) ? it->second : NodeTest("", "");
}

std::string NodeTest::getType() const {
    return type;
}

void NodeTest::print() const {
    std::cout << "Type: " << type << ", Value: " << value << std::endl;
    for (const auto& child : children) {
        child.print();
    }
}

void NodeTest::updateChildMap() {
    childMap.clear();
    for (const auto& child : children) {
        childMap[child.getType()] = child;
    }
}

