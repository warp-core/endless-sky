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

#include "Angle.h"
#include "Command.h"
#include "DataFile.h"
#include "DataNode.h"
#include "Engine.h"
#include "FileWriter.h"
#include "GameData.h"
#include "NetworkShip.h"
#include "ServerInfo.h"
#include "Ship.h"
#include "System.h"
#include "Files.h"
#include "PacketWriter.h"
#include "PacketReader.h"
#include "Point.h"
#include "PlayerInfo.h"

#include <bitset>
#include <iostream>

using namespace std;



void Server::LoadServerList(vector<Server *> &list, PlayerInfo &player)
{
    DataFile servers(Files::Config() + "servers.txt");
    
	for(const DataNode &node : servers)
	{
        list.emplace_back(new Server(node.Token(0), static_cast<int>(node.Value(1)), player));
	}
}



void Server::SaveServerList(const vector<Server *> &list)
{
    FileWriter out(Files::Config() + "servers.txt");
	
    for(unsigned int i = 0; i < list.size(); i++)
	   out.Write(list[i]->host, list[i]->port);
}



// Constructor
Server::Server(const string &host, unsigned int port, PlayerInfo &player)
    :host(host), port(port), player(player)
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
    :host(t.host), port(t.port), player(player)
{
    // host = t.host;
    // port = t.port;
    address = new IPaddress();
    *address = *(t.address);
    cached = new ServerInfo();
    *cached = *(t.cached);
    // player = t.player;
    
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
    :player(other.player)
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



void Server::Query()
{
    // Prepare the query packet
    UDPpacket* packet = SDLNet_AllocPacket(1);
    packet->address = *address;
    packet->len = 1;
    packet->data = new Uint8('Q');
    
    int timeSent = SDL_GetTicks();
    
    SendUDPPacket(*packet, [timeSent, this] (UDPpacket &recv) {
        if(recv.data[0] == 'R')
        {
            // Calulate ping
            unsigned int ping = (SDL_GetTicks() - timeSent) / 2;
            
            // Get the length of the name
            Uint8 nameLength = recv.data[1];
            char* name = new char[nameLength];
            
            // Read the name
            for(int i = 0; i < nameLength; i++) {
                name[i] = recv.data[2 + i];
            }
            
            // Get the other information
            unsigned int online      = recv.data[2 + nameLength];
            unsigned int maxPlayers  = recv.data[2 + nameLength + 1];
            unsigned int hasPassword = recv.data[2 + nameLength + 2];
            
            // Update cached server info
            cached->ping = ping;
            cached->alive = true;
            cached->name = string(name);
            
            cached->occupancey = online;
            cached->maxPlayers = maxPlayers;
            cached->hasPassword = hasPassword;
        }
    });
    
    // Free memory used for query packet
    SDLNet_FreePacket(packet);
    packet = nullptr;
}



void Server::Ping()
{
    // Prepare the ping packet
    UDPpacket* ping = SDLNet_AllocPacket(1);
    ping->address = *address;
    ping->len = 1;
    ping->data = new Uint8('P');
    
    int timeSent = SDL_GetTicks();
    
    // Send ping packet
    SendUDPPacket(*ping, [timeSent, this] (UDPpacket &recv) {
        // Calculate ping
        unsigned int dt = -1u;
        if(recv.data[0] == 'P')
            dt = (SDL_GetTicks() - timeSent) / 2;
        
        this->cached->ping = dt;
        this->cached->alive = (dt != -1u);
    });
    
    // Free memory used for ping packet
    SDLNet_FreePacket(ping);
    ping = nullptr;
}



const ServerInfo &Server::GetStatus() const
{
    return *cached;
}



void Server::RequestPlayerInfo(PlayerInfo &player, function<void (bool)> callback)
{
	unsigned int uuid;
	cout << "Enter UUID\n?" << endl;
	cin >> uuid;
	// Prepare the query packet
	UDPpacket* packet = SDLNet_AllocPacket(5);
	packet->len = 5;
	packet->data[0] = 'U';
	SDLNet_Write32(uuid, packet->data + 1);

	SendUDPPacket(*packet, [this, &player, callback](UDPpacket &recv){
	if(recv.data[0]=='L')
		{
			PacketReader reader(recv);
			
			player.Load(reader);
			player.ApplyChanges();
			
			shared_ptr<NetworkShip> flagship;
			
			unsigned int uuid = static_cast<unsigned int>(reader.begin()->Value(0)) << 16;
			cout << "Player uuid" << (uuid >> 16) << endl;
			for(const shared_ptr<Ship> ship : player.Ships())
			{
					uuid++;

					NetworkShip *ns = new NetworkShip(*this, ship, uuid);
					
					// Constructs a std::pair<unsigned int, NetworkShip *>
					ships.emplace(uuid, ns);

					// Constructs a std::shared_ptr<NetworkShip>
					playerShips.emplace_back(ns);

					cout << "DONE" << endl;
			}
			
			callback(true);
		}
		else
		{
			callback(false);
		}
	});
	SDLNet_FreePacket(packet);
	packet = nullptr;
}



void Server::PushRemoteShip(const shared_ptr<Ship> &ship)
{
	unsigned int uuid = 38;
	char bytes[5];
	bytes[0] = 10;
	SDLNet_Write32(uuid, bytes + 1);

	string words = string(bytes, 5);
	PacketWriter writer(words);


	SendUDPPacket(*(writer.Flush()));
	cout << "Sent departure info to server." << endl;
}

/*void Server::EnterSystem()
{
	for(shared_ptr<NetworkShip> ship : playerShips)
	{
		unsigned int uuid = ship->Uuid();
		char bytes[5];
		bytes[0] = 10;
		SDLNet_Write32(uuid, bytes + 1);

		string words = string(bytes, 5);
		PacketWriter writer(words);

		writer.Write(ship->GetSystem()->Name());

		writer.Write("pos", ship->Position().X(), ship->Position().Y());
		writer.Write("vel", ship->Velocity().X(), ship->Velocity().Y());
		writer.Write("rot", ship->Facing().Get());

		SendUDPPacket(*(writer.Flush()));
		cout << "Sent departure info to server." << endl;
	}
}*/



void Server::Hyperspace(NetworkShip &ship, const System &target)
{
	cout << "Requesting permission to enter " << target.Name() << endl;
	//char bytes[5];
	//bytes[0] = 10;
	//SDLNet_Write32(ship.Uuid(), bytes + 1);
	
	//string words = string(bytes, 5);
	PacketWriter writer("\x0A");
	
	writer.Write(target.Name());
	
	SendUDPPacket(*(writer.Flush()), [this, &ship, &target](UDPpacket &recv){
		if(recv.data[0] == 'H')
		{
			cout << "Cleared into system, reading manifest" << endl;
			ship.ClearHyperspace();
			PacketReader reader(recv, 1);
			cout << "Populating System" << endl;
			for(const DataNode &node : reader)
			{
				if(node.Size() == 0) continue;
				unsigned int uuid = static_cast<unsigned int>(node.Value(0));
				if(ships.count(uuid)==0)
				{
					// TODO Send UDP packet of form 0x0B 0x00 0x00 0x00 0x00 where the last
					// four bytes are inquired about
					UDPpacket *packet = SDLNet_AllocPacket(5);
					packet->data[0] = 0x0B;
					SDLNet_Write32(uuid, packet->data + 1);
					packet->len = 5;
					cout << "Unknown ship " << uuid << endl;
					// Server::Step() handles this just like it is a new ship
					SendUDPPacket(*packet);
				} else {
					if(!ships[uuid]->GetShip()->IsYours())
						ships[uuid]->UpdateStatus(node);
					else
						cout << "You own ship " << uuid << endl;
				}
			}
			// TODO Prepare the ships for dropping out of warp
		}
	});
}


vector<shared_ptr<Ship>> Server::GetRemoteShips() const
{
	// TODO This method should not ask the server if there have been new ships,
	// the server will tell us if we have new ships in Server::Step(), this method just moves them
	// from the new ship list to the ship list;
	//cout << "Searching for new ships" << endl;
}



const vector<shared_ptr<Ship>> Server::GetNewShips() const
{
	vector<shared_ptr<Ship>> temp = newShips;

	newShips.clear();

	return temp;
}


void Server::EnterSystem(const System &system) {
	for(shared_ptr<NetworkShip> ship : playerShips)
		if(player.GetPlanet() && ship->GetShip()->GetSystem() == player.GetSystem() && !ship->GetShip()->IsDisabled())
			EnterSystem(*ship, system);
}

void Server::EnterSystem(NetworkShip &ship, const System &system) {
	UUID uuid = ship.Uuid();
	
	char bytes[5];
	bytes[0] = 0x0C;
	SDLNet_Write32(uuid, bytes + 1);

	string words = string(bytes, 5);
	PacketWriter writer(words);

	ship.WriteStatus(writer);

	SendUDPPacket(*(writer.Flush()));
	cout << "Sent arrival info to server." << endl;
}



void Server::Step()
{
	// TODO Don't keep reallocating this, use one packet the entire time.
    UDPpacket* recv = SDLNet_AllocPacket(32768);
    
    int numrecv = SDLNet_UDP_Recv(socket, recv);
    
    // Recived packet
    while(numrecv)
    {
		switch(recv->data[0]) {
		case 0x0B:
		{
			PacketReader reader(*recv, 1);
			
			UUID squak = static_cast<UUID>(reader.begin()->Value(0));
			UUID flag  = static_cast<UUID>(reader.begin()->Value(1));

			// If we do not have info on the flagship, create a placeholder. It will be updated
			// when the flagship warps into the system
			// TODO breaks down when parent ship is in a different sector, but info needed for ai control
			if(flag != 0 && !ships.count(flag))
				ships[flag] = new NetworkShip(*this, shared_ptr<Ship>(new Ship()), flag);
			
			if(!ships.count(squak))
				// TODO give NetworkShip a constructer that takes a NetworkShip * and 
				//      deletes it in the destructor (or give networkship a constructor
				//      that makes the ship itself
				ships[squak] = new NetworkShip(*this, shared_ptr<Ship>(new Ship()), squak);
			
			// If this ship is a flagship, flagship == nullptr
			NetworkShip *ship     = ships[squak];
			NetworkShip *flagship = flag != 0 ? ships[flag] : nullptr;
			
			ship->GetShip()->Load(*(++(reader.begin())));
			ship->GetShip()->FinishLoading();

			if(flagship)
				ship->GetShip()->SetParent(flagship->GetShip());

			ship->UpdateStatus(*(++(++(reader.begin()))));

			newShips.emplace_back(ship->GetShip());

			cout << "Got data for ship " << squak << endl;
		}
		break;
		// Ship has entered a system
		case 0x0C:
		{
			cout << "CASE 0x0C" << endl;
			UUID uuid = SDLNet_Read32(recv->data + 1);
			PacketReader reader(*recv, 5);
			string system = reader.begin()->Token(0);
			// TODO Check if this message regards a system we have an intrest in (One of the ships we have authoritiy over is in that system)
			if(ships.count(uuid))
			{
				cout << uuid << "@" << ships[uuid] << " " << ships.count(uuid) << endl;
				if(ships[uuid])
				if(ships[uuid]->GetShip())
				if(!ships[uuid]->GetShip()->IsYours())
				{
					cout << "new ship" << endl;
					ships[uuid]->UpdateStatus(*(reader.begin()));
					newShips.emplace_back(ships[uuid]->GetShip());
				}
			}
			else
			{
				cout << "Unknown ship " << uuid << endl;
				UDPpacket *packet = SDLNet_AllocPacket(5);
				packet->len = 5;
				packet->data[0] = 0x0B;
				
				SDLNet_Write32(uuid, packet->data + 1);
				SendUDPPacket(*packet);
			}
			cout << "Ship " << uuid << " has entered " << system << endl;
		}
		break;
		case 'W':
		{
			cout << "New ships have entered this sector" << endl;
			Uint8 n = recv->data[1];
			unsigned int uuids[n];
			for(int i = 0; i < n; i++)
				uuids[n] = SDLNet_Read32(4 * n + 2 + recv->data);
			
			// 0 - H
			// 1 - Number of ships in packet
			// 2 - uuid0
			// 6 - uuid1
			// ...
			
			unsigned int uuid = 17;
			
			PacketReader reader(*recv, 4 * n + 2);
			const DataNode &shipStat = *(reader.begin());
			const DataNode &shipData = *(shipStat.begin());
			const DataNode &positionData = *(++shipStat.begin());
			
			// Validates that input is as expected
			if(shipStat.Size() == 1         && shipStat.Token(0) == "shipStat"
				&& shipData.Size() == 2     && shipData.Token(0) == "ship"
				&& positionData.Size() == 1 && positionData.Token(0) == "stat")
			{
				// Will be freed by Serveer::~Server
				Ship *s = new Ship();
				
				s->Load(shipData);
				s->FinishLoading();
				
				NetworkShip *ns = new NetworkShip(*this, shared_ptr<Ship>(s), 0);

				ships.emplace(uuid, ns);
				
				ns->UpdateStatus(positionData);
			}	
		}
		break;
		default:
		{
			cout << "Recived Packet" << endl;
			if(waiting.waiting) {
				waiting.callback(*recv);
				waiting.waiting = false;
			}
		}
		break;
		}
   	numrecv = SDLNet_UDP_Recv(socket, recv);
	}
    
    // Handle Timeout
    if(SDL_GetTicks() - waiting.timeSent > waiting.timeoutMillis) {
        waiting.waiting = false;
    }
     
    // Get a list of all the commands applied to the players ship this turn
    if(player.Flagship() != nullptr)
    {
        Command commands = player.Flagship()->Commands();
        Uint32 command = commands.Serialize();
        
        UDPpacket* packet = SDLNet_AllocPacket(5);
        packet->len = 5;
        packet->data[0] = 'C';
        
        SDLNet_Write32(command, packet->data + 1);
        
        // Send packet
		//SendUDPPacket(*packet,[](UDPpacket &packet){});
        
        /*if(!hasSentShip)
        {
            // Packet writer will handle memory allocation for this packet
            UDPpacket *packet;
            
            // Because PacketWriter flushes in the deconstructor, we need to
            // force it to go out of scope.
            {
                PacketWriter file(packet, 'S');
                player.Flagship()->Save(file);
            }
            
            SendUDPPacket(*packet, [](UDPpacket &packet){});
            
            // Free resources, FreePacket delete[]s the packets data buffer
            SDLNet_FreePacket(packet);
            
            hasSentShip = true;
        }*/
    }
    // Free memory used for ping packet
    SDLNet_FreePacket(recv);
    recv = nullptr;
}


void Server::SendUDPPacket(UDPpacket &packet) {
    packet.address = *address;
    
    int numsent = SDLNet_UDP_Send(socket, -1, &packet);
    if(!numsent)
        cerr << "Failed to send packet: " << SDLNet_GetError() << endl;
}



void Server::SendUDPPacket(UDPpacket &packet, function<void (UDPpacket &)> callback) {
    packet.address = *address;
    
    int numsent = SDLNet_UDP_Send(socket, -1, &packet);
    if(!numsent)
        cerr << "Failed to send packet: " << SDLNet_GetError() << endl;
    
    waiting.waiting = true;
    waiting.callback = callback;
    waiting.timeSent = SDL_GetTicks();
    waiting.timeoutMillis = 2000;
}



void Server::Init()
{
    socket = SDLNet_UDP_Open(0);
    if(!socket)
        cerr << "Failed to open socket." << SDLNet_GetError() << endl;
    else
        if(!SDLNet_UDP_Bind(socket, -1, address));
            cerr << "Failed to bind channel: " << SDLNet_GetError();
}
