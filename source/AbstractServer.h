#ifndef ABSTRACT_SERVER_H_
#define ABSTRACT_SERVER_H_

#include <memory>
#include <vector>

#include "Angle.h"
#include "Point.h"

class Angle;
class Point;
class Ship;
class System;
class ShipStatus;
class NetworkShip;

class AbstractServer {
public:
    virtual ~AbstractServer() {}
    virtual void Step() = 0;

	// FIXME Deprecated
	// Gets a list of all the ships in the players sector
	virtual std::vector<std::shared_ptr<Ship>> GetRemoteShips() const {};
	virtual const std::vector<std::shared_ptr<Ship>> GetNewShips() const {};
	
	// Gets a list of any new ships in the system
	virtual std::vector<std::shared_ptr<Ship>> GetNewShips(const System &system) const {};

	// Gets a list of all of the ships already in the players system
	virtual std::vector<std::shared_ptr<Ship>> SystemStatus(const System &system) {};

	// FIXME Deprecated
	// Sends a remote ship to be added to the server's world
	virtual void PushRemoteShip(const std::shared_ptr<Ship> &ship) {};

	// Tells the server that the player is entering a system (From a planet)
	virtual void EnterSystem(const System &system) {};
	virtual void EnterSystem(NetworkShip &ship, const System &system) {};

	// Tells the server that a ship is entering a system
	virtual void Hyperspace(NetworkShip &ship, const System &system) {};

	// Test if this server is the host
	virtual bool IsHost() const {};
};

class ShipStatus {
public:
	std::shared_ptr<Ship> ship;
	Point position;
	Point velocity;
	Angle rotation;
};

#endif
