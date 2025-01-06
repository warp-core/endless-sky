/* SpriteSet.h
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

#pragma once

#include "../Set.h"
#include "Sprite.h"

#include <map>
#include <mutex>
#include <set>
#include <string>
#include <vector>



// Class for storing sprites, and for getting the sprite associated with a given
// name. If a sprite has not been loaded yet, this will still return an object
// but with no OpenGL textures associated with it (so it will draw nothing).
class SpriteSet {
public:
	// Get a pointer to the sprite data with the given name.
	const Sprite *Get(const std::string &name) const;
	Sprite *Modify(const std::string &name);

	explicit operator Set<Sprite> &() { return sprites; }
	explicit operator const Set<Sprite> &() const { return sprites; }

	const std::vector<const Sprite *> &MoonSprites() const;
	const std::vector<const Sprite *> &StationSprites() const;
	const std::vector<const Sprite *> &GiantSprites() const;
	const std::vector<const Sprite *> &PlanetSprites() const;
	const std::vector<const Sprite *> &StarSprites() const;

	// Inspect the sprite map and warn if some images contain no data.
	void CheckReferences() const;


private:
	mutable Set<Sprite> sprites;
	mutable std::vector<const Sprite *> moonSprites;
	mutable std::vector<const Sprite *> stationSprites;
	mutable std::vector<const Sprite *> giantSprites;
	mutable std::vector<const Sprite *> planetSprites;
	mutable std::vector<const Sprite *> starSprites;
	mutable std::mutex modifyMutex;
};
