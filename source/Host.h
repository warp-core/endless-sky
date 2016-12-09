#ifndef HOST_H_
#define HOST_H_

#include "AbstractServer.h"

#include <SDL2/SDL_net.h>

#include <functional>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

class DataNode;
class DataWriter;
class LocalServerConnector;
class PlayerInfo;
class Ship;
class NetworkShip;
class System;


typedef unsigned int UUID;
class Host : public AbstractServer {
public:
	static Host &GetInstance();
	static AbstractServer *GetConnector();
	static bool IsActive();
	static void Start(std::string name, int port, int maxPlayers);

public:
	void Step();
	//std::vector<std::shared_ptr<Ship>> GetShipsInSystem(const System &system);

private:
	static Host *instance;
	static LocalServerConnector *connector;

private:
	Host(std::string name, int port, int maxPlayers);
	~Host();

	void Init(int port);
	void EnterSystem(const DataNode &shipStat);

	void PushShips(std::vector<std::shared_ptr<Ship>> ships) const;

	void PushShips(const System &system,      const IPaddress &address) const;
	void PushShips(const std::string &system, const IPaddress &address) const;

	void WriteShip(const Ship &ship, DataWriter &writer) const;

	void AnswerQuery(IPaddress &address) const;

	PlayerInfo *LoadPlayer(const UUID &uuid);

	void ProcessClient(const UUID &uuid, unsigned int data);

	void GlobalBroadcast(UDPpacket *packet) const;
	void SystemBroadcast(UDPpacket *packet, const std::string &system) const;
	void SendPacket(const IPaddress &address, UDPpacket *packet) const;

private:
    std::string name;
    
    Uint8 ingame = 0;
    Uint8 maxPlayers = 32;
    Uint8 hasPassword = 0;
    
    UDPsocket socket;
    UDPpacket *packet;
	
	UUID uuidCounter = 0;
	UUID shipUuidCounter = 0;

	// IPaddress reference is allocated on the stack, when the packet goes out of scope, so does the address
	// So we need to copy the ip
	std::unordered_map<UUID, std::pair<IPaddress, PlayerInfo *>> clients;
	
	// UUID is owner ID and ship ID, ie, xxxxxxx
	std::unordered_map<UUID, NetworkShip *> ships;
	
	mutable std::vector<std::shared_ptr<Ship>> newShips;
    mutable std::vector<std::shared_ptr<Ship>> remoteShips;

};



#endif
