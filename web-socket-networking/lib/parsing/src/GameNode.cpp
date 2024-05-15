#include "GameNode.h"

GameNode::GameNode() {
    key = ""; 
    value = ""; 
}

GameNode::GameNode(std::string_view k, std::string_view v) {
    key = k; 
    value = v; 
}

void GameNode::setKey(std::string_view k) {
    key = k;
}

void GameNode::setValue(std::string_view v) {
    value = v; 
}

std::string_view GameNode::getKey() const {
    return key; 
}

std::string_view GameNode::getValue() const {
    return value; 
}

int GameNode::getNumChildren() const {
    return children.size(); 
}

std::shared_ptr<GameNode> GameNode::getChildWithKey(std::string_view k) const {
    for(int i=0; i < getNumChildren(); i++) {
        if(children[i]->getKey() == k) {
            return children[i]; 
        }
    }
    return nullptr; 
}

void GameNode::clearAllChildren() {
    children.clear(); 
}

void GameNode::addChild(std::shared_ptr<GameNode> newChild) {
    children.push_back(newChild); 
}
