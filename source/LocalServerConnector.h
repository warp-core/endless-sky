#ifndef LOCAL_SERVER_CONNECTOR_H_
#define LOCAL_SERVER_CONNECTOR_H_

#include "AbstractServer.h"

class Host;

class LocalServerConnector : public AbstractServer {
friend class Host;

public:
	
    virtual void Step() override;

	// Gets a list of all the ships in the players sector
	virtual std::vector<std::shared_ptr<Ship>> GetRemoteShips() const override;


	// Sends a remote ship to be added to the server's world
	virtual void PushRemoteShip(const std::shared_ptr<Ship> &ship) override;

	// Test if this server is the host
	virtual bool IsHost() const override { return false; }
	

private:
	LocalServerConnector(std::vector<std::shared_ptr<Ship>> &hostShips, std::vector<std::shared_ptr<Ship>> &newShips);
	
	void GiveShips(std::vector<std::shared_ptr<Ship>> ships);

private:
	std::vector<std::shared_ptr<Ship>> &hostShips;
	std::vector<std::shared_ptr<Ship>> &newShips;
	
	mutable std::vector<std::shared_ptr<Ship>> cachedShips;
};



#endif
