#include "NetworkShip.h"
#include "AbstractServer.h"
#include "System.h"
#include "DataWriter.h"
#include "DataNode.h"
#include "GameData.h"

#include <iostream>
#include <memory>

using namespace std;



NetworkShip::NetworkShip(AbstractServer &server, shared_ptr<Ship> ship,unsigned int uuid)
	: uuid(uuid), ship(ship), server(server)
{
	// Fake virtual functions
	ship->getHSPermission = [this](){return GetHSPermission();};
	ship->enterSystem = [&server, this](){server.EnterSystem(*this, *(GetShip()->GetSystem()));};

	cout << "Constructed ship " << (uuid >> 16) << ":" << (uuid & 0xFFFF) << endl;
}



unsigned int NetworkShip::Uuid() const
{
	return uuid;
}



shared_ptr<Ship> NetworkShip::GetShip() const
{
	return ship;
}



bool NetworkShip::GetHSPermission()
{
	// Get up to speed first
	// Once ready for warp in send message
	if(!hyperspacePermission)
	{
		if(!waitingForHSPermission)
		{
			server.Hyperspace(*this, *(ship->GetTargetSystem()));
			waitingForHSPermission = true;
		}
		return false;
	}
	
	hyperspacePermission = false;
	waitingForHSPermission = false;
	return true;
}



void NetworkShip::ClearHyperspace()
{
	hyperspacePermission = true;
}

void NetworkShip::UpdateStatus(const DataNode &node)
{
	string system = node.Token(1);
	
	Point position;
	Point velocity;
	Angle rotation;

	for(auto prop : node)
	{
		if(prop.Token(0) == "pos" && prop.Size()==3)
		{
			double x = prop.Value(1);
			double y = prop.Value(2);
			cout << "Position (" << x << ", " << y << ")" << endl;
			position = Point(x, y);
		}
		else if(prop.Token(0) == "vel" && prop.Size() == 3)
		{
			double x = prop.Value(1);
			double y = prop.Value(2);
			cout << "Velocity <" << x << ", " << y << ">" << endl;
			velocity = Point(x, y);
		}
		else if(prop.Token(0) == "rot" && prop.Size() == 2)
		{
			double ang = prop.Value(1);
			cout << "Rotation " << ang << endl;
			rotation = ang;
		}
		else if(prop.Size()==0)
			continue;	
	}
	ship->SetSystem(GameData::Systems().Get(system));
	ship->Place(position, velocity, rotation);
}

void NetworkShip::WriteStatus(DataWriter &writer) const
{
	writer.Write(uuid, ship->GetSystem()->Name());

	writer.BeginChild();

	writer.Write("pos", ship->Position().X(), ship->Position().Y());
	writer.Write("vel", ship->Velocity().X(), ship->Velocity().Y());
	writer.Write("rot", ship->Facing().Get());
	
	writer.EndChild();
}
