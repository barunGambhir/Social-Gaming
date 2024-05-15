#include "ExtractWhatItem.h"
#include <sstream>
#include <iostream>

ExtractWhatItem::ExtractWhatItem() = default; 

void ExtractWhatItem::setCallChain(std::string_view _chain) {
    //the passed in chain looks like: abc.chieu.rheviu.vieuv
    std::string chain(_chain); 
    std::stringstream ss(Helper::removeWrappingQuotes(chain));
    std::string part;
    while (std::getline(ss, part, '.')) {
        callChain.push_back(part);
    }
}

void ExtractWhatItem::setBuiltin(std::string_view b) {
    builtin = b;
}

void ExtractWhatItem::setArguments(std::string_view _args) {
    std::string args(_args);
    args = Helper::removeWrappingParentheses(args);
    std::stringstream ss(args);
    std::string part;
    while(std::getline(ss, part, ',')) {
        arguments.push_back(part);
    }
}

std::vector<std::string> ExtractWhatItem::getCallChain() {
    return callChain; 
}

std::string ExtractWhatItem::getBuiltin() {
    return builtin; 
}

std::vector<std::string> ExtractWhatItem::getArguments() {
    return arguments; 
}

void ExtractWhatItem::print() {
    std::cout << "Call chain:"; 
    for(auto p: callChain) {
        std::cout << p; 
    }
    std::cout << std::endl;

    std::cout << "Builtin:"; 
    std::cout << builtin << std::endl;
    
    std::cout << "Arguments:"; 
    for(auto p: arguments) {
        std::cout << p;
    }
    std::cout << std::endl;
}
