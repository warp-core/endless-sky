#include "LocalServerConnector.h"

#include "Host.h"

using namespace std;



LocalServerConnector::LocalServerConnector(
		vector<shared_ptr<Ship>> &shipVector, 
		vector<shared_ptr<Ship>> &newShips)
	: hostShips(shipVector), newShips(newShips)
{
}



void LocalServerConnector::Step()
{

}



vector<shared_ptr<Ship>> LocalServerConnector::GetRemoteShips() const
{
	return hostShips;
}



/*vector<shared_ptr<Ship>> LocalServerConnector::GetNewShips() const
{
	vector<shared_ptr<Ship>> copy = cachedShips;
	cachedShips.clear();
	return copy;
}*/



void LocalServerConnector::PushRemoteShip(const shared_ptr<Ship> &ship) {
	//newShips.push_back(ship);
	//hostShips.push_back(ship);
}



void LocalServerConnector::GiveShips(vector<shared_ptr<Ship>> ships)
{
	for(const shared_ptr<Ship> ship : ships)
		newShips.emplace_back(ship);
}
