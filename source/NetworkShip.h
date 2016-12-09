#ifndef NETWORK_SHIP_H_
#define NETWORK_SHIP_H_

#include "Ship.h"

#include <memory>

class AbstractServer;

class NetworkShip {
public:
	NetworkShip(AbstractServer &server, std::shared_ptr<Ship> ship, unsigned int uuid);
	unsigned int Uuid() const;
	void ClearHyperspace();

	void UpdateStatus(const DataNode &node);
	void WriteStatus(DataWriter &writer) const;

	std::shared_ptr<Ship> GetShip() const;

private:
	bool GetHSPermission();


private:
	bool constructed = false;
	const unsigned int uuid;
	AbstractServer &server;
	std::shared_ptr<Ship> ship;
	
	bool waitingForHSPermission = false;
	bool hyperspacePermission = false;
};

#endif
