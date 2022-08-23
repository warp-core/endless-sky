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
	map<string, shared_ptr<ImageSet>> FindImages(const vector<string> &sources)
	{
		auto images = map<string, shared_ptr<ImageSet>>{};
		for(const string &source : sources)
		{
			// All names will only include the portion of the path that comes after
			// this directory prefix.
			string directoryPath = source + "images/";
			size_t start = directoryPath.size();

			vector<string> imageFiles = Files::RecursiveList(directoryPath);
			for(string &path : imageFiles)
				if(ImageSet::IsImage(path))
				{
					string name = ImageSet::Name(path.substr(start));

					shared_ptr<ImageSet> &imageSet = images[name];
					if(!imageSet)
						imageSet.reset(new ImageSet(name));
					imageSet->Add(std::move(path));
				}
		}
		return images;
	}
}



future<void> GameAssets::Load(const vector<string> &sources, int options)
{
	return TaskQueue::Run([this, sources, options]
		{
			if(!(options & OnlyData))
			{
				LoadImages(sources);
				LoadSounds(sources);
			}

			auto future = objects.Load(sources, options & Debug);
		});
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



void GameAssets::LoadImages(const std::vector<std::string> &sources)
{
	// Now, read all the images in all the path directories. For each unique
	// name, only remember one instance, letting things on the higher priority
	// paths override the default images.
	// From the name, strip out any frame number, plus the extension.
	for(const auto &it : FindImages(sources))
	{
		// This should never happen, but just in case:
		if(!it.second)
			continue;

		// Reduce the set of images to those that are valid.
		it.second->ValidateFrames();
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



void GameAssets::LoadSounds(const std::vector<std::string> &sources)
{
	for(const string &source : sources)
	{
		string root = source + "sounds/";
		vector<string> files = Files::RecursiveList(root);

		for(const string &path : files)
		{
			// Sanity check on the path length.
			if(path.length() < root.length() + 4)
				continue;
			string ext = path.substr(path.length() - 4);

			// Music sound files are loaded when needed.
			if(ext == ".mp3" || ext == ".MP3")
			{
				string name = path.substr(root.length(), path.length() - root.length() - 4);
				music[name] = path;
			}
			// Regular sound files are loaded into memory for faster access.
			else if(ext == ".wav" || ext == ".WAV")
			{
				// The "name" of the sound is its full path within the "sounds/"
				// folder, without the ".wav" or "~.wav" suffix.
				size_t end = path.length() - 4;
				if(path[end - 1] == '~')
					--end;
				auto name = path.substr(root.length(), end - root.length());
				TaskQueue::Run([this, name = std::move(name), path = path]
					{
						if(!sounds.Modify(name)->Load(path, name))
							Logger::LogError("Unable to load sound \"" + name + "\" from path: " + path);
						++soundProgress.first;
					});
				++soundProgress.second;
			}
		}
	}
}
