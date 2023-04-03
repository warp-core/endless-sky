/* BodyTemplate.cpp
Copyright (c) 2016 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "BodyTemplate.h"

#include "DataNode.h"
#include "DataWriter.h"
#include "GameData.h"
#include "Mask.h"
#include "MaskManager.h"
#include "Random.h"
#include "Screen.h"
#include "Sprite.h"
#include "SpriteSet.h"

#include <algorithm>
#include <cmath>

using namespace std;



// Check that this Body has a sprite and that the sprite has at least one frame.
bool BodyTemplate::HasSprite() const
{
	return (sprite && sprite->Frames());
}



// Access the underlying Sprite object.
const Sprite *BodyTemplate::GetSprite() const
{
	return sprite;
}



// Get the width of this object, in world coordinates (i.e. taking zoom and scale into account).
double BodyTemplate::Width() const
{
	return static_cast<double>(sprite ? (.5f * zoom) * scale * sprite->Width() : 0.f);
}



// Get the height of this object, in world coordinates (i.e. taking zoom and scale into account).
double BodyTemplate::Height() const
{
	return static_cast<double>(sprite ? (.5f * zoom) * scale * sprite->Height() : 0.f);
}



// Get the farthest a part of this sprite can be from its center.
double BodyTemplate::Radius() const
{
	return .5 * Point(Width(), Height()).Length();
}



// Which color swizzle should be applied to the sprite?
int BodyTemplate::GetSwizzle() const
{
	return swizzle;
}



// Zoom factor. This controls how big the sprite should be drawn.
double BodyTemplate::Zoom() const
{
	return max(zoom, 0.f);
}



double BodyTemplate::Scale() const
{
	return static_cast<double>(scale);
}



// Load the sprite specification, including all animation attributes.
void BodyTemplate::LoadSprite(const DataNode &node)
{
	if(node.Size() < 2)
		return;
	sprite = SpriteSet::Get(node.Token(1));

	// The only time the animation does not start on a specific frame is if no
	// start frame is specified and it repeats. Since a frame that does not
	// start at zero starts when the game started, it does not make sense for it
	// to do that unless it is repeating endlessly.
	for(const DataNode &child : node)
	{
		if(child.Token(0) == "frame rate" && child.Size() >= 2 && child.Value(1) >= 0.)
			frameRate = child.Value(1) / 60.;
		else if(child.Token(0) == "frame time" && child.Size() >= 2 && child.Value(1) > 0.)
			frameRate = 1. / child.Value(1);
		else if(child.Token(0) == "delay" && child.Size() >= 2 && child.Value(1) > 0.)
			delay = child.Value(1);
		else if(child.Token(0) == "scale" && child.Size() >= 2 && child.Value(1) > 0.)
			scale = static_cast<float>(child.Value(1));
		else if(child.Token(0) == "start frame" && child.Size() >= 2)
		{
			frameOffset += static_cast<float>(child.Value(1));
			startAtZero = true;
		}
		else if(child.Token(0) == "random start frame")
			randomize = true;
		else if(child.Token(0) == "no repeat")
		{
			repeat = false;
			startAtZero = true;
		}
		else if(child.Token(0) == "rewind")
			rewind = true;
		else
			child.PrintTrace("Skipping unrecognized attribute:");
	}

	if(scale != 1.f)
		GameData::GetMaskManager().RegisterScale(sprite, Scale());
}



// Save the sprite specification, including all animation attributes.
void BodyTemplate::SaveSprite(DataWriter &out, const string &tag) const
{
	if(!sprite)
		return;

	out.Write(tag, sprite->Name());
	out.BeginChild();
	{
		if(frameRate != static_cast<float>(2. / 60.))
			out.Write("frame rate", frameRate * 60.);
		if(delay)
			out.Write("delay", delay);
		if(scale != 1.f)
			out.Write("scale", scale);
		if(randomize)
			out.Write("random start frame");
		if(!repeat)
			out.Write("no repeat");
		if(rewind)
			out.Write("rewind");
	}
	out.EndChild();
}



// Set the sprite.
void BodyTemplate::SetSprite(const Sprite *sprite)
{
	this->sprite = sprite;
}



// Set the color swizzle.
void BodyTemplate::SetSwizzle(int swizzle)
{
	this->swizzle = swizzle;
}



BodyTemplate::BodyTemplate(const Sprite *sprite, const float zoom, const bool randomize)
	: zoom(zoom), sprite(sprite), randomize(randomize)
{
}



// Set the frame rate of the sprite. This is used for objects that just specify
// a sprite instead of a full animation data structure.
void BodyTemplate::SetFrameRate(float framesPerSecond)
{
	frameRate = framesPerSecond / 60.f;
}



// Add the given amount to the frame rate.
void BodyTemplate::AddFrameRate(float framesPerSecond)
{
	frameRate += framesPerSecond / 60.f;
}
