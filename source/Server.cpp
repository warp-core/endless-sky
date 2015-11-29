/* Server.cpp
Copyright (c) 2015 by Zach Anderson

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "Server.h"

#include "DataFile.h"
#include "DataNode.h"
#include "FileWriter.h"
#include "ServerInfo.h"
#include "Files.h"

#include <iostream>

using namespace std;



void Server::LoadServerList(vector<Server> &list)
{
    DataFile servers(Files::Config() + "servers.txt");
    
	for(const DataNode &node : servers)
	{
        list.emplace_back(node.Token(0), static_cast<int>(node.Value(1)));
	}
}



void Server::SaveServerList(const vector<Server> &list)
{
    FileWriter out(Files::Config() + "servers.txt");
	
    for(unsigned int i = 0; i < list.size(); i++)
	   out.Write(list[i].host, list[i].port);
}



// Constructor
Server::Server(const string &host, unsigned int port)
    :host(host), port(port)
{
    cached = new ServerInfo();
    cached->ip = host;
    cached->alive = false;
    cached->name = host;
    cached->port = port;
    
    address = new IPaddress();
    if(SDLNet_ResolveHost(address, host.c_str(), port) == 0)
    {
        Init();
        Query();
    }
    else
    {
        cerr << "Failed to resolve host " << host << " on port "<< port << endl;
        socket = nullptr;
    }
}



// Copy
Server::Server(const Server &t)
{
    host = t.host;
    port = t.port;
    address = new IPaddress();
    *address = *(t.address);
    cached = new ServerInfo();
    *cached = *(t.cached);
    
    // Don't need to query because we copied the cached data
    Init();
}



// Destructor
Server::~Server()
{
    if(address != nullptr)
    {
        delete address;
        address = nullptr;
    }
    
    if(socket != nullptr)
    {
        SDLNet_UDP_Unbind(socket, 0);
        SDLNet_UDP_Close(socket);
        socket = nullptr;
    }
    
    if(cached != nullptr)
    {
        delete cached;
        cached = nullptr;
    }
}



// Move
Server::Server(Server&& other)
{
    host = other.host;
    port = other.port;
    address = other.address;
    socket = other.socket;
    cached = other.cached;

    other.host = "";
    other.port = 0;
    other.address = nullptr;
    other.socket  = nullptr;
    other.cached  = nullptr;
}



const ServerInfo &Server::Query()
{
    // Prepare the query packet
    UDPpacket* packet = SDLNet_AllocPacket(1);
    packet->address = *address;
    packet->len = 1;
    packet->data = new Uint8('Q');
    
    int timeSent = SDL_GetTicks();
    
    // Send query packet
    int numsent = SDLNet_UDP_Send(socket, -1, packet);
    if(!numsent)
        cerr << "Failed to send query: " << SDLNet_GetError() << endl;
    
    // Free memory used for query packet
    SDLNet_FreePacket(packet);
    packet = nullptr;
    
    // Allocate room for reply
    UDPpacket *recv = SDLNet_AllocPacket(32);
    
    // TODO This should not block, waiting for response should be moved into
    //      the draw loop of the appropriate panel.
    // Wait for reply
    int numrecv;
    do {
        numrecv = SDLNet_UDP_Recv(socket, recv);
    } while(numrecv == 0 && SDL_GetTicks() - timeSent < 2000); // Timeout after 2 seconds
    
    // Recived packet
    if(numrecv)
    {
        if(recv->data[0] == 'R')
        {
            // Calulate ping
            unsigned int ping = (SDL_GetTicks() - timeSent) / 2;
            
            // Get the length of the name
            Uint8 nameLength = recv->data[1];
            char* name = new char[nameLength];
            
            // Read the name
            for(int i = 0; i < nameLength; i++) {
                name[i] = recv->data[2 + i];
            }
            
            // Get the other information
            unsigned int online      = recv->data[2 + nameLength];
            unsigned int maxPlayers  = recv->data[2 + nameLength + 1];
            unsigned int hasPassword = recv->data[2 + nameLength + 2];
            
            // Update cached server info
            cached->ping = ping;
            cached->alive = true;
            cached->name = string(name);
            
            cached->occupancey = online;
            cached->maxPlayers = maxPlayers;
            cached->hasPassword = hasPassword;
        }
    }
    
    // Release the resources
    SDLNet_FreePacket(recv);
    recv = nullptr;
    
    return *cached;
}



unsigned int Server::Ping()
{
    // Prepare the ping packet
    UDPpacket* ping = SDLNet_AllocPacket(1);
    ping->address = *address;
    ping->len = 1;
    ping->data = new Uint8('P');
    
    int timeSent = SDL_GetTicks();
    
    // Send ping packet
    int numsent = SDLNet_UDP_Send(socket, -1, ping);
    if(!numsent)
        cerr << "Failed to send ping: " << SDLNet_GetError() << endl;
    
    // TODO This should not block, waiting for response should be moved into
    //      the draw loop of the appropriate panel.
    // Wait for reply, timeout after 2 seconds
    int numrecv;
    while(numrecv == 0 && SDL_GetTicks() - timeSent < 2000)
        numrecv = SDLNet_UDP_Recv(socket, ping);
    
    // Calculate ping
    unsigned int dt = -1u;
    if(numrecv)
        if(ping->data[0] == 'P')
            dt = (SDL_GetTicks() - timeSent) / 2;
    
    // Free memory used for ping packet
    SDLNet_FreePacket(ping);
    ping = nullptr;
    
    cached->ping = dt;
    return dt;
}



const ServerInfo &Server::GetStatus() const
{
    return *cached;
}



void Server::Init()
{
    socket = SDLNet_UDP_Open(0);
    if(!socket)
    {
        cerr << "Failed to open socket." << SDLNet_GetError() << endl;
    }
    else
    {
        int channel = SDLNet_UDP_Bind(socket, -1, address);
        
        if(channel == -1)
            cerr << "Failed to bind channel: " << SDLNet_GetError();
    }
}
