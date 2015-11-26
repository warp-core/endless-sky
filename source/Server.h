/* Server.h
Copyright (c) 2015 by Zach Anderson

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#ifndef SERVER_H
#define SERVER_H

#include <SDL2/SDL_net.h>

#include <string>
#include <vector>

class ServerInfo;



// Class that represents a remote game server. Provides methods to query the
// status of the remote server and manages the socket through which all
// communication to the remote server will flow.
class Server {
public:
    static void LoadServerList(std::vector<Server> &list);
    static void SaveServerList(const std::vector<Server> &list);
    
public:
    Server(const std::string &host, unsigned int port);
    Server(const Server &t);
    ~Server();
    
    Server(Server&& other);
    
    // Queries the remote server over the network and updates the cached server status.
    const ServerInfo &Query();
    
    // Updates the cached ping and returns the new value
    unsigned int Ping();
    
    // Returns the cached status of this server without any network communication.
    const ServerInfo &GetStatus() const;
    
    
private:
    void Init();
    
    
private:
    std::string host;
    unsigned int port;
    
    IPaddress* address;
    UDPsocket socket;
    
    ServerInfo* cached;
};



#endif
