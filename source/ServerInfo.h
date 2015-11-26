/* ServerInfo.h
Copyright (c) 2015 by Zach Anderson

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#ifndef SERVER_INFO_H
#define SERVER_INFO_H

#include <string>


// Structure to hold remote server status information.
class ServerInfo {
public:
    std::string name;
    std::string ip;
    unsigned int port;
    unsigned int ping;
    unsigned int occupancey;
    unsigned int maxPlayers;
    bool hasPassword;
    bool alive;
};



#endif
