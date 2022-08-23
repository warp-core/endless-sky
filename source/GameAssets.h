/* GameAssets.h
Copyright (c) 2022 by quyykk

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#ifndef GAME_ASSETS_H_
#define GAME_ASSETS_H_

#include "SoundSet.h"
#include "SpriteSet.h"
#include "UniverseObjects.h"

#include <future>
#include <map>
#include <memory>
#include <string>
#include <vector>

class ImageSet;



// This class contains the game assets needed to play the game: The game data, the images and the sounds.
class GameAssets {
	// GameData currently is the orchestrating controller for all game definitions.
	friend class GameData;
public:
	static constexpr int None = 0x0;
	static constexpr int Debug = 0x1;
	static constexpr int OnlyData = 0x2;


public:
	GameAssets() noexcept = default;

	// Asset objects cannot be copied.
	GameAssets(const GameAssets &) = delete;
	GameAssets(GameAssets &&) = delete;
	GameAssets &operator=(const GameAssets &) = delete;
	GameAssets &operator=(GameAssets &&) = delete;

	// Load all the assets from the given sources.
	std::future<void> Load(const std::vector<std::string> &sources, int options = None);
	// Determine the fraction of assets read from disk.
	double GetProgress() const;

	// Begin loading a sprite that was previously deferred. Currently this is
	// done with all landscapes to speed up the program's startup.
	std::future<void> Preload(const Sprite *sprite);


private:
	void LoadImages(const std::vector<std::string> &sources);
	void LoadSounds(const std::vector<std::string> &sources);


private:
	// The game assets.
	std::map<std::string, std::string> music;
	SoundSet sounds;
	SpriteSet sprites;
	UniverseObjects objects;

	// Tracks the progress of loading the sprites and sounds when the game starts.
	std::pair<int, int> spriteProgress{};
	std::pair<int, int> soundProgress{};

	// Deferred sprite loading support.
	std::map<const Sprite *, std::shared_ptr<ImageSet>> deferred;
	std::map<const Sprite *, int> preloaded;
};



#endif
