#include "../include/Player.h"
//#include "Server.h" // Include Server.h here to avoid circular dependency in .h files

Player::Player(std::string_view uuid, std::string_view name) : playerName{name}, id{uuid} {
}

std::string Player::getName() const {
    return playerName;
}

std::string Player::getId() const {
    return id;
}
