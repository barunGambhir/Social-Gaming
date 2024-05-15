#include "Environment.h"



void Environment::init(std::string name) {
    auto res = env.find(name);
    if (res != env.end()) {
        return; // Varaible already exists, could change to throw exception.
    }

    env.emplace(name, std::any());
}



std::any Environment::get(const std::string& name) {
    auto res = env.find(name);

    if (res != env.end()) {
        return res->second;
    }

    if (enclosing != nullptr) {
        return enclosing->get(name);
    }

    return std::any(); // Variable not found, could change to throw exception
}



void Environment::set(const std::string& name, std::any value) {
    auto res = env.find(name);
    if (res == env.end()) {
        return; // Variable not found, could change to throw exception
    }

    (*res).second = value;
}