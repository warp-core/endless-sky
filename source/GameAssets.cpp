/* GameAssets.cpp
Copyright (c) 2022 by quyykk

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "GameAssets.h"

#include "Files.h"
#include "ImageSet.h"
#include "Logger.h"
#include "MaskManager.h"
#include "Sprite.h"
#include "TaskQueue.h"

#include <map>
#include <memory>
#include <utility>

using namespace std;



namespace {

}



future<void> GameAssets::Load(const vector<string> &sources, int options)
{
	return TaskQueue::Run([this, sources, options]
		{
			if(!(options & OnlyData))
			{
				ImageMap images;
				SoundMap sounds;

				for(const string &source : sources)
				{
					FindImages(images, source + "images/");
					FindSounds(sounds, source + "sounds/");
				}
				LoadImages(images);
				LoadSounds(sounds);
			}

			objects.Load(sources, options & Debug);
		});
}



future<void> GameAssets::LoadObjects(const string &path, int options)
{
	return TaskQueue::Run([this, path, options]
		{
			objects.LoadFrom(path, options & Debug);
		});
}



void GameAssets::LoadSounds(const string &path, SoundMap sounds)
{
	FindSounds(sounds, path);
	LoadSounds(sounds);
}



void GameAssets::LoadSprites(const string &path, ImageMap images)
{
	FindImages(images, path);
	LoadImages(images);
}



void GameAssets::FindImages(ImageMap &images, const string &path)
{
	for(string &file : Files::RecursiveList(path))
		if(ImageSet::IsImage(file))
		{
			// All names will only include the portion of the path that comes after
			// this directory prefix.
			string name = ImageSet::Name(file.substr(path.size()));

			shared_ptr<ImageSet> &imageSet = images[name];
			if(!imageSet)
				imageSet.reset(new ImageSet(name));
			imageSet->Add(std::move(file));
		}
}



void GameAssets::FindSounds(SoundMap &sounds, const string &path)
{
	for(const auto &file : Files::RecursiveList(path))
	{
		// Sanity check on the path length.
		if(file.length() < path.length() + 4)
			continue;
		string ext = file.substr(file.length() - 4);

		// Music sound files are loaded when needed.
		if(ext == ".mp3" || ext == ".MP3")
		{
			string name = file.substr(path.length(), file.length() - path.length() - 4);
			sounds[name + ext] = file;
		}
		// Regular sound files are loaded into memory for faster access.
		else if(ext == ".wav" || ext == ".WAV")
		{
			// The "name" of the sound is its full path within the "sounds/"
			// folder, without the ".wav" or "~.wav" suffix.
			size_t end = file.length() - 4;
			if(file[end - 1] == '~')
				--end;
			auto name = file.substr(path.length(), end - path.length());
			sounds[name + ext] = file;
		}
	}
}



double GameAssets::GetProgress() const
{
	double sound = static_cast<double>(soundProgress.first) / soundProgress.second;
	double sprite = static_cast<double>(spriteProgress.first) / spriteProgress.second;
	double data = objects.GetProgress();

	if(sound == 1. && sprite == 1. && data == 1.)
		return 1.;
	return sound * .2 + sprite * .7 + data * .1;
}



// Begin loading a sprite that was previously deferred. Currently this is
// done with all landscapes to speed up the program's startup.
future<void> GameAssets::Preload(const Sprite *sprite)
{
	// Make sure this sprite actually is one that uses deferred loading.
	auto dit = deferred.find(sprite);
	if(!sprite || dit == deferred.end())
		return future<void>();

	// If this sprite is one of the currently loaded ones, there is no need to
	// load it again. But, make note of the fact that it is the most recently
	// asked-for sprite.
	map<const Sprite *, int>::iterator pit = preloaded.find(sprite);
	if(pit != preloaded.end())
	{
		for(pair<const Sprite * const, int> &it : preloaded)
			if(it.second < pit->second)
				++it.second;

		pit->second = 0;
		return future<void>();
	}

	// This sprite is not currently preloaded. Check to see whether we already
	// have the maximum number of sprites loaded, in which case the oldest one
	// must be unloaded to make room for this one.
	const string &name = sprite->Name();
	pit = preloaded.begin();
	while(pit != preloaded.end())
	{
		++pit->second;
		if(pit->second >= 20)
		{
			sprites.Modify(name)->Unload();
			pit = preloaded.erase(pit);
		}
		else
			++pit;
	}

	// Now, load all the files for this sprite.
	preloaded[sprite] = 0;
	auto image = dit->second;
	return TaskQueue::Run([image] { image->Load(); },
			[this, image] { image->Upload(sprites.Modify(image->Name())); });
}



auto GameAssets::SaveSnapshot() const -> Snapshot
{
	Snapshot snapshot;
	snapshot.colors = objects.colors;
	snapshot.conversations = objects.conversations;
	snapshot.effects = objects.effects;
	snapshot.events = objects.events;
	snapshot.fleets = objects.fleets;
	snapshot.galaxies = objects.galaxies;
	snapshot.governments = objects.governments;
	snapshot.hazards = objects.hazards;
	snapshot.interfaces = objects.interfaces;
	snapshot.minables = objects.minables;
	snapshot.missions = objects.missions;
	snapshot.news = objects.news;
	snapshot.outfits = objects.outfits;
	snapshot.persons = objects.persons;
	snapshot.phrases = objects.phrases;
	snapshot.planets = objects.planets;
	snapshot.ships = objects.ships;
	snapshot.systems = objects.systems;
	snapshot.tests = objects.tests;
	snapshot.testDataSets = objects.testDataSets;
	snapshot.shipSales = objects.shipSales;
	snapshot.outfitSales = objects.outfitSales;
	snapshot.sounds = static_cast<const Set<Sound> &>(sounds);
	snapshot.sprites = static_cast<const Set<Sprite> &>(sprites);
	snapshot.music = music;
	return snapshot;
}



void GameAssets::Revert(const Snapshot &snapshot)
{
	objects.colors.Revert(snapshot.colors);
	objects.conversations.Revert(snapshot.conversations);
	objects.effects.Revert(snapshot.effects);
	objects.events.Revert(snapshot.events);
	objects.fleets.Revert(snapshot.fleets);
	objects.galaxies.Revert(snapshot.galaxies);
	objects.governments.Revert(snapshot.governments);
	objects.hazards.Revert(snapshot.hazards);
	objects.interfaces.Revert(snapshot.interfaces);
	objects.minables.Revert(snapshot.minables);
	objects.missions.Revert(snapshot.missions);
	objects.news.Revert(snapshot.news);
	objects.outfits.Revert(snapshot.outfits);
	objects.persons.Revert(snapshot.persons);
	objects.phrases.Revert(snapshot.phrases);
	objects.planets.Revert(snapshot.planets);
	objects.ships.Revert(snapshot.ships);
	objects.systems.Revert(snapshot.systems);
	objects.tests.Revert(snapshot.tests);
	objects.testDataSets.Revert(snapshot.testDataSets);
	objects.shipSales.Revert(snapshot.shipSales);
	objects.outfitSales.Revert(snapshot.outfitSales);
	music.Revert(snapshot.music);

	// Sounds and sprites need to be unloaded before they are removed or replaced.
	// They need to be manually loaded!
	static_cast<Set<Sound> &>(sounds).RevertNoOverwrite(snapshot.sounds);
	static_cast<Set<Sprite> &>(sprites).RevertNoOverwrite(snapshot.sprites);
}



void GameAssets::LoadSounds(const SoundMap &sounds)
{
	for(const auto &[name, sound]: sounds)
	{
		string ext = name.substr(name.length() - 4);

		// Music sound files are loaded when needed.
		if(ext == ".mp3" || ext == ".MP3")
			*music.Get(name.substr(0, name.length() - 4)) = sound;
		// Regular sound files are loaded into memory for faster access.
		else if(ext == ".wav" || ext == ".WAV")
		{
			// The "name" of the sound is its full path within the "sounds/"
			// folder, without the ".wav" or "~.wav" suffix.
			auto filename = name.substr(0, name.length() - 4);
			TaskQueue::Run([this, name = std::move(filename), file = sound]
				{
					if(!this->sounds.Modify(name)->Load(file, name))
						Logger::LogError("Unable to load sound \"" + name + "\" from: " + file);
					++soundProgress.first;
				});
			++soundProgress.second;
		}
	}
}



void GameAssets::LoadImages(const ImageMap &images)
{
	// Now, read all the images in all the path directories. For each unique
	// name, only remember one instance, letting things on the higher priority
	// paths override the default images.
	// From the name, strip out any frame number, plus the extension.
	for(const auto &it : images)
	{
		// This should never happen, but just in case:
		if(!it.second)
			continue;

		// Reduce the set of images to those that are valid.
		it.second->ValidateFrames();

		// If this image is already loaded, don't load it again.
		if(it.second->Path(false) == sprites.Get(it.second->Name())->Path(false)
				&& it.second->Path(true) == sprites.Get(it.second->Name())->Path(true))
			continue;

		// For landscapes, remember all the source files but don't load them yet.
		if(ImageSet::IsDeferred(it.first))
			deferred[sprites.Get(it.first)] = it.second;
		else
		{
			TaskQueue::Run([image = it.second] { image->Load(); },
					[this, image = it.second] { image->Upload(sprites.Modify(image->Name())); ++spriteProgress.first; });
			++spriteProgress.second;
		}
	}
}
