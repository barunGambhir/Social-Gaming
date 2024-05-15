/*
An environment stores information about variables in a given game. It considers a variable a name.
To support scope and variable shadowing, an environment has an enclosing environemnt. It will search itself for a variable first.
If it is not found it will ask its enclosing environment instead. 
*/

#pragma once
#include "SGData.h"
#include <unordered_map>
#include <string>

class Environment {
public:
    Environment() : enclosing(nullptr) {}; 
    Environment(Environment* enc, std::unordered_map<std::string, SGData> e) : enclosing{enc}, varMap{std::move(e)} {};
    Environment(Environment* enc) : enclosing{enc} {};
    Environment(const Environment&) = delete;

    void init(std::string_view name);
    SGData& get(const std::string_view name);
    void set(const std::string_view name, SGData& value);

private:
    Environment* enclosing;
    std::unordered_map<std::string, SGData> varMap;
};