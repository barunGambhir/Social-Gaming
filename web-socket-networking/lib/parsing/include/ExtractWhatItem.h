#pragma once

#include <any>
#include <memory>
#include <vector>
#include <string_view>
#include "Helper.h"

class ExtractWhatItem {
private:
    std::vector<std::string> callChain;
    std::string builtin; 
    std::vector<std::string> arguments;
public:
    ExtractWhatItem(); 
    void setCallChain(std::string_view chain); 
    void setBuiltin(std::string_view b); 
    void setArguments(std::string_view args); 

    std::vector<std::string> getCallChain();
    std::string getBuiltin(); 
    std::vector<std::string> getArguments();

    void print(); 
};