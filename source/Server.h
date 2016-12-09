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

#include "AbstractServer.h"

#include <SDL2/SDL_net.h>

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

class Angle;
class ServerInfo;
class Ship;
class System;
class NetworkShip;
class PlayerInfo;
class Point;

typedef unsigned int UUID;

// Class that represents a remote game server. Provides methods to query the
// status of the remote server and manages the socket through which all
// communication to the remote server will flow.
class Server : public AbstractServer {
public:
    static void LoadServerList(std::vector<Server *> &list, PlayerInfo &player);
    static void SaveServerList(const std::vector<Server *> &list);
    
public:
    Server(const std::string &host, unsigned int port, PlayerInfo &player);
    Server(const Server &t);
    ~Server();
    
    Server(Server&& other);
    
    // Queries the remote server over the network and updates the cached server status.
    void Query();
    
    // Updates the cached ping and returns the new value
    void Ping();
    
    // Returns the cached status of this server without any network communication.
    const ServerInfo &GetStatus() const;
    
	// Join the server
    void RequestPlayerInfo(PlayerInfo &player, std::function<void (bool)> callback);
    
	// Gets a list of all the new ships in this sector
	virtual std::vector<std::shared_ptr<Ship>> GetRemoteShips() const override;

	virtual const std::vector<std::shared_ptr<Ship>> GetNewShips() const override;
	
	// Places a ship in a remote system
	virtual void PushRemoteShip(const std::shared_ptr<Ship> &ship) override;

	virtual void EnterSystem(NetworkShip &ship, const System &system) override;
	virtual void EnterSystem(const System &system) override;

	virtual void Hyperspace(NetworkShip &ship, const System &system) override;

    virtual void Step() override;

	virtual bool IsHost() const override { return false; }
	
	void EnterSystem(UUID &shipID);
    
private:
    class WaitingStruct {
    public:
        bool waiting = false;
        std::function<void (UDPpacket &)> callback;
        unsigned int timeSent = 0;
        unsigned int timeoutMillis = 0;
    };
    
private:
    void SendUDPPacket(UDPpacket &packet);
    void SendUDPPacket(UDPpacket &packet, std::function<void (UDPpacket &)> callback);
    void Init();
    
    
private:
    std::string host;
    unsigned int port;
    PlayerInfo &player;
    
    IPaddress *address;
    UDPsocket socket;
    
    ServerInfo *cached;
	
	// The definitive home for all ships that use the network
	std::map<unsigned int, NetworkShip *> ships;
	// Points to ships in the map
	std::vector<std::shared_ptr<NetworkShip>> playerShips;

	mutable std::vector<std::shared_ptr<Ship>> newShips;
    
    WaitingStruct waiting;
    
    bool hasSentShip = false;
};



#endif
