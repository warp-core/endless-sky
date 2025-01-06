/* SpriteSet.cpp
Copyright (c) 2014 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "SpriteSet.h"

#include "../Set.h"
#include "../Files.h"
#include "../GameData.h"
#include "../Logger.h"
#include "Sprite.h"

using namespace std;



const Sprite *SpriteSet::Get(const string &name) const
{
	lock_guard<mutex> guard(modifyMutex);

	auto it = sprites.Find(name);
	if(!it)
	{
		it = sprites.Get(name);
		*it = Sprite(name);
	}
	return it;
}



Sprite *SpriteSet::Modify(const string &name)
{
	return const_cast<Sprite *>(Get(name));
}



const vector<const Sprite *> &SpriteSet::MoonSprites() const
{
	return moonSprites;
}



const vector<const Sprite *> &SpriteSet::StationSprites() const
{
	return stationSprites;
}



const vector<const Sprite *> &SpriteSet::GiantSprites() const
{
	return giantSprites;
}


const vector<const Sprite *> &SpriteSet::PlanetSprites() const
{
	return planetSprites;
}



const vector<const Sprite *> &SpriteSet::StarSprites() const
{
	return starSprites;
}



void SpriteSet::CheckReferences() const
{
	moonSprites.clear();
	stationSprites.clear();
	giantSprites.clear();
	starSprites.clear();

	for(const auto &pair : sprites)
	{
		const auto &name = pair.first;
		const Sprite &sprite = pair.second;
		if(sprite.Height() == 0 && sprite.Width() == 0)
			// Landscapes are allowed to still be empty.
			if(pair.first.compare(0, 5, "land/") != 0)
				Logger::LogError("Warning: image \"" + pair.first + "\" is referred to, but has no pixels.");
		// Sort the planet sprites for the system editor.
		if(!name.compare(0, 7, "planet/"))
		{
			// Ignore ringworlds, panels, dyson and wormholes.
			if(name.find("ringworld") != string::npos)
				continue;
			if(name.find("wormhole") != string::npos)
				continue;
			if(name.find("panels") != string::npos)
				continue;
			if(name.find("dyson") != string::npos)
				continue;
			if(name.find("gas3-c") != string::npos)
				// This is the Coalition satellite planet bruh.
				continue;
			if(name.find("-rouge") != string::npos)
			{
				// This is actually a star!
				starSprites.push_back(&sprite);
				continue;
			}
			if(name.find("station") != string::npos)
			{
				stationSprites.push_back(&sprite);
				continue;
			}

			auto radius = sprite.Width() / 2. - 4.;

			// Sort the sprites based on radius.
			if(radius <= 50.)
				moonSprites.push_back(&sprite);
			else if(radius >= 120.)
				giantSprites.push_back(&sprite);
			else
				planetSprites.push_back(&sprite);
		}
		else if(!name.compare(0, 5, "star/"))
		{
			// Ignore novas, blackholes, neutron stars, etc.
			if(name.find("nova") != string::npos)
				continue;
			if(name.find("black-hole") != string::npos)
				continue;
			if(name.find("neutron") != string::npos)
				continue;
			if(name.find("wr") != string::npos)
				continue;
			if(name.find("carbon") != string::npos)
				continue;

			starSprites.push_back(&sprite);
		}
	}
}
