#include "Host.h"

#include "Angle.h"
#include "DataWriter.h"
#include "DataReader.h"
#include "DataFile.h"
#include "DataNode.h"
#include "Files.h"
#include "GameData.h"
#include "LocalServerConnector.h"
#include "NetworkShip.h"
#include "PacketReader.h"
#include "PacketWriter.h"
#include "PlayerInfo.h"
#include "Point.h"
#include "Ship.h"
#include "System.h"

#include <algorithm>
#include <iostream>

using namespace std;

Host *Host::instance;
LocalServerConnector *Host::connector;

Host &Host::GetInstance()
{
	return *instance;
}



AbstractServer *Host::GetConnector()
{
	return connector;
}



bool Host::IsActive()
{
	return instance != nullptr;
}



void Host::Start(string name, int port, int maxPlayers)
{
	if(instance != nullptr)
	{
		delete instance;
		delete connector;
	}
	
	instance = new Host(name, port, maxPlayers);
	connector = new LocalServerConnector(instance->remoteShips, instance->newShips);
}



Host::Host(std::string name, int port ,int maxPlayers)
    :name(name), maxPlayers(maxPlayers)
{
    Init(port);
}



Host::~Host()
{
	//TODO
	// - Save all client profiles
	// - Clean up resources
	// - Dump AI info
	SDLNet_FreePacket(packet);
	packet = nullptr;
}



void Host::Init(int port)
{
    socket = SDLNet_UDP_Open(port);
    if(!socket) {
        printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        exit(2);
    }
    printf("Opened socket\n");
	packet = SDLNet_AllocPacket(1024);
}



void Host::Step()
{
    int numrecv = SDLNet_UDP_Recv(socket, packet);
    while (numrecv)
    {
        switch(*(packet->data))
        {
            // echo the packet for a Ping
            case 'P':
			{
                SDLNet_UDP_Send(socket, -1, packet);
                break;
			}

			// respond to a server Query	
            case 'Q':
			{
				AnswerQuery(packet->address);
				break;
 			}

			// handle remote Commands 
            case 'C':
			{
				const UUID uuid = SDLNet_Read32(packet->data + 1);
				const Uint32 data = SDLNet_Read32(packet->data + 5);
				ProcessClient(uuid, data);
				break; 
			}

			// request to join the game without uuid
            case 'I':
			{
				UUID uuid = uuidCounter;
				uuidCounter++;
				
				// Tell the client we don't have a save for them
				UDPpacket *reply = SDLNet_AllocPacket(5);
				reply->len = 5;
				reply -> data[0] = 'N';
				SDLNet_Write32(uuid, packet->data + 1);
				
				SendPacket(packet->address, reply);
			}
			break;

			// request to join the game with uuid
			case 'U':
			{
				UUID uuid = SDLNet_Read32(packet->data + 1);
				
				PlayerInfo *player = LoadPlayer(uuid);
				
				if(player)
				{
					cout << "Located record for player" << endl;
					clients.emplace(uuid, make_pair(packet->address, player));
					
					PacketWriter writer('L');
					writer.Write(uuid);
					
					vector<shared_ptr<Ship>> s = player->Ships();
					
					// Assign uuids to every ship in the players fleet, player.Ships()
					UUID id = uuid << 16;
					for(shared_ptr<Ship> ship : s) 
					{
						id++;

						if(!ships.count(id))
							ships.emplace(id, new NetworkShip(*this, ship, id));
					}
					
					player->Save(writer);
					
					SendPacket(packet->address, writer.Flush());
				}
				else
				{
					cout << "No save for player " << uuid << endl;
				}
			}
			break;
			
			// Player info from a new player
			case 'N':
			{
				PacketReader reader(*packet);
				UUID uuid = reader.begin()->Value(0);
				
				PlayerInfo *p = new PlayerInfo();
				p->Load(*(++reader.begin()));
				clients[uuid] = make_pair(packet->address, p);
				
				cout << "Loaded player " << uuid << endl;
			}
			break;
			
			// A client is requesting data on a ship
			case 0x0B:
			{
				UUID uuid = SDLNet_Read32(packet->data + 1);
				cout << "A client requested data on ship " << uuid << endl;
				if(ships.count(uuid))
				{
					shared_ptr<Ship> ship = ships[uuid]->GetShip();
					PacketWriter writer("\x0B");
					// Write static data
					writer.Write(uuid);
					ship->Save(writer);

					// Write current status
					ships[uuid]->WriteStatus(writer);

					SendPacket(packet->address, writer.Flush());
				}
			}
			break;

			// A client is requesting the status of ships in a system
			case 0x0A:
			{
				PacketReader reader(*packet, 1);
				string system = reader.begin()->Token(0);
				cout << "Someone" << " is requesting status of " << system << endl;
				
				vector<shared_ptr<NetworkShip>> shipsInSystem;
				
				for(pair<UUID, NetworkShip *> kv : ships)
				{
					cout << kv.first << endl;
					// FIXME SegFault in NetworkShip::GetShip()
					//       seems like its after all of the ships have been exahausted
					//       out of bounds?
					if(kv.second->GetShip())
						if(kv.second->GetShip()->GetSystem())
							// TODO check if parked
							if(kv.second->GetShip()->GetSystem()->Name() == system)
							{
								cout << kv.second->Uuid() << endl;
								// Have to copy pair so it doesn't go out of scope
								shipsInSystem.emplace_back(kv.second);
							}
				}

				PacketWriter writer("H");
				
				for(shared_ptr<NetworkShip> ship : shipsInSystem)
					ship->WriteStatus(writer);
				
				SendPacket(packet->address, writer.Flush());
			}
			break;
			
			// A ship has entered a system
			case 0x0C:
			{
				UUID uuid = SDLNet_Read32(packet->data + 1);
				
				
				PacketReader reader(*packet, 5);
				
				ships[uuid]->UpdateStatus(*(reader.begin()));
				
				cout << "Ship: " << uuid << " has entered " << ships[uuid]->GetShip()->GetSystem()->Name() << endl;
				
				// Send this ship to all clients
				GlobalBroadcast(packet);
				break;
			}

            default:
                cout << "Malformed packet" << endl;
                break;
        }
		numrecv = SDLNet_UDP_Recv(socket, packet);
    }
	// Broadcast all of the new ships to the appropriate clients
	// TODO Ships are echoed back to their owner, need to filter out
	//PushShips(newShips);
	connector->GiveShips(newShips);
	newShips.clear();

	// If it has been more than x time, broadcast all ships again
	// PushShips(remoteShips);
}


void Host::EnterSystem(const DataNode &shipStat)
{

}



void Host::PushShips(vector<shared_ptr<Ship>> ships) const
{
	if(ships.empty())
		return;

	cout << "Pushing new ships" << endl;
	unordered_multimap<string, const Ship &> map;
	set<string> systems;

	for(const shared_ptr<Ship> ship : ships )
	{
		map.emplace(ship->GetSystem()->Name(), *ship);
		systems.insert(ship->GetSystem()->Name());
	}

	for(const string system : systems)
	{
		PacketWriter writer('H');

		auto list = map.equal_range(system);
		for(;list.first != list.second; list.first++) {
			WriteShip(list.first->second, writer);
		}

		SystemBroadcast(writer.Flush(), system);
	}
}



void Host::PushShips(const System &system, const IPaddress &address) const
{
	PushShips(system.Name(), address);
}

void Host::PushShips(const string &system, const IPaddress &address) const
{
	/*vector<shared_ptr<Ship>> ships;
	
	for(const shared_ptr<Ship> ship : remoteShips)
		if(ship->GetSystem()->Name() == system)
			ships.push_back(ship);

	PacketWriter writer('H');
	
	for(const shared_ptr<Ship> ship : ships)
		WriteShip(*ship, writer);

	SendPacket(address, writer.Flush());
*/}

void Host::WriteShip(const Ship &ship, DataWriter &writer) const
{
	writer.Write("pos", ship.Position().X(), ship.Position().Y());
	writer.Write("vel", ship.Velocity().X(), ship.Velocity().Y());
	writer.Write("rot", ship.Facing().Get());
}


void Host::AnswerQuery(IPaddress &address) const
{
	Uint8 nameLength = name.size();

	// Prepare the data
	Uint8* buffer = new Uint8[7 + nameLength];
	buffer[0] = 'R';

	buffer[1] = nameLength;
	for(int i = 0; i < nameLength; i++)
		buffer[i+2]=name[i];
	
	buffer[2+nameLength] = ingame;
	buffer[2+nameLength+1] = maxPlayers;
	buffer[2+nameLength+2] = hasPassword;
	
	// Prepare the packet
	UDPpacket* echo = new UDPpacket();
	echo->channel = -1;
	echo->data = buffer;
	echo->len = 7 + nameLength;
	echo->maxlen = 7 + nameLength;
	echo->address = address;
	
	cout << "SENT QUERY RESPONSE" << endl;
	
	// Send the packet
	SDLNet_UDP_Send(socket, -1, echo);
}



PlayerInfo *Host::LoadPlayer(const UUID &uuid)
{
	cout << "Player " << uuid << " is joining the game" << endl;

	// Check if player is currently cached
	if(clients.count(uuid) == 0)
	{
		// Freed by Host destructor
		string savePath = Files::Config() + "server/players/" + to_string(uuid);

		if(Files::Exists(savePath))
		{
			cout << "Located save file for player" << uuid << ": " << savePath << endl;
			
			PlayerInfo *p = new PlayerInfo();
			p->Load(DataFile(savePath));
			
			return p;
		}
		cout << "No save exists for player: " << savePath << endl;
		return nullptr;
	}
	cout << "Player " << uuid << " already cached" << endl;
	return clients[uuid].second;
}



void Host::ProcessClient(const UUID &uuid, unsigned int data)
{
	if(clients.count(uuid) != 0) {
		const double conversion = (2.0 / ((1 << 16) - 1));
		
		unsigned int commands = data & 0xFFFF;
		unsigned int turnInt = (data >> 16) & 0xFFFF;
		
		double turn = (turnInt * conversion) - 1;
		if(turnInt == 32768)
			turn = 0;
		
		Command c;
		c.UnSerialize(commands, turn);
		clients[uuid].second->Flagship()->SetCommands(c);
	}
}



void Host::GlobalBroadcast(UDPpacket *packet) const
{
	// TODO use a broadcast channel
	for(auto const keyVal : clients)
		SendPacket(keyVal.second.first, packet);
}



void Host::SystemBroadcast(UDPpacket *packet, const string &system) const
{
	for(auto const keyVal : clients)
		if(keyVal.second.second->GetSystem()->Name() == system)
			SendPacket(keyVal.second.first, packet);
}



void Host::SendPacket(const IPaddress &address, UDPpacket *packet) const
{
	packet->address = address;
	SDLNet_UDP_Send(socket, -1, packet);
}
