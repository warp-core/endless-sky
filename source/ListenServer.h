/* ListenServer.h
Copyright (c) 2015 by Zach Anderson

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#ifndef LISTEN_SERVER_H_
#define LISTEN_SERVER_H_

#include <SDL2/SDL_net.h>

#include <string>



// Handles all logic for the creation and management of the network sockets
// for a multiplayer server.
class ListenServer {
public:
    explicit ListenServer(std::string name);
    void Step();
    
    
private:
    void Init(int port);
    
    
private:
    std::string name;
    
    Uint8 ingame = 0;
    Uint8 maxPlayers = 32;
    Uint8 hasPassword = 0;
    
    UDPsocket socket;
};



#endif
