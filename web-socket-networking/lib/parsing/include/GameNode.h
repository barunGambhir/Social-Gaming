#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <memory>

class GameNode {
private:
    std::string_view key; 
    std::string_view value; 
    std::vector<std::shared_ptr<GameNode>> children; 
public:
    GameNode(); 
    GameNode(std::string_view k, std::string_view v); 

    void setKey(std::string_view k); 
    void setValue(std::string_view v); 
    std::string_view getKey() const; 
    std::string_view getValue() const; 
    int getNumChildren() const; 
    std::shared_ptr<GameNode> getChildWithKey(std::string_view k) const; 
    void clearAllChildren(); 
    void addChild(std::shared_ptr<GameNode> newChild); 
}; 