#ifndef PLAYER_H
#define PLAYER_H

//struct networking::Connection; // Forward declaration of Connection struct
//#include "Server.h"
#include <string>

//using namespace networking;

class Player {
public:
    Player(std::string_view name, std::string_view playerId);

    std::string getName() const;

    std::string getId() const;

private:
    std::string playerName;
    std::string id;
};

#endif
