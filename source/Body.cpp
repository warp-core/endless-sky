/* Body.cpp
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

#include "Body.h"

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



// Constructor, based on a Sprite.
Body::Body(const Sprite *sprite, Point position, Point velocity, Angle facing, double zoom)
	: BodyTemplate(sprite, zoom, true), position(position), velocity(velocity), angle(facing)
{
}



// Constructor, based on the animation from another Body object.
Body::Body(const Body &sprite, Point position, Point velocity, Angle facing, double zoom)
{
	*this = sprite;
	this->position = position;
	this->velocity = velocity;
	this->angle = facing;
	this->zoom = zoom;
}



// Get the frame index for the given time step. If no time step is given, this
// will return the frame from the most recently given step.
float Body::GetFrame(int step) const
{
	if(step >= 0)
		SetStep(step);

	return frame;
}



// Get the mask for the given time step. If no time step is given, this will
// return the mask from the most recently given step.
const Mask &Body::GetMask(int step) const
{
	if(step >= 0)
		SetStep(step);

	static const Mask EMPTY;
	int current = round(frame);
	if(!sprite || current < 0)
		return EMPTY;

	const vector<Mask> &masks = GameData::GetMaskManager().GetMasks(sprite, Scale());

	// Assume that if a masks array exists, it has the right number of frames.
	return masks.empty() ? EMPTY : masks[current % masks.size()];
}



// Position, in world coordinates (zero is the system center).
const Point &Body::Position() const
{
	return position;
}



// Velocity, in pixels per second.
const Point &Body::Velocity() const
{
	return velocity;
}



// Direction this Body is facing in.
const Angle &Body::Facing() const
{
	return angle;
}



// Unit vector in the direction this body is facing. This represents the scale
// and transform that should be applied to the sprite before drawing it.
Point Body::Unit() const
{
	return angle.Unit() * (.5 * Zoom());
}



// Check if this object is marked for removal from the game.
bool Body::ShouldBeRemoved() const
{
	return shouldBeRemoved;
}



// Store the government here too, so that collision detection that is based
// on the Body class can figure out which objects will collide.
const Government *Body::GetGovernment() const
{
	return government;
}



void Body::PauseAnimation()
{
	++pause;
}



// Mark this object to be removed from the game.
void Body::MarkForRemoval()
{
	shouldBeRemoved = true;
}



// Mark this object to not be removed from the game.
void Body::UnmarkForRemoval()
{
	shouldBeRemoved = false;
}



// Set the current time step.
void Body::SetStep(int step) const
{
	// If the animation is paused, reduce the step by however many frames it has
	// been paused for.
	step -= pause;

	// If the step is negative or there is no sprite, do nothing. This updates
	// and caches the mask and the frame so that if further queries are made at
	// this same time step, we don't need to redo the calculations.
	if(step == currentStep || step < 0 || !sprite || !sprite->Frames())
		return;
	currentStep = step;

	// If the sprite only has one frame, no need to animate anything.
	float frames = sprite->Frames();
	if(frames <= 1.f)
	{
		frame = 0.f;
		return;
	}
	float lastFrame = frames - 1.f;
	// This is the number of frames per full cycle. If rewinding, a full cycle
	// includes the first and last frames once and every other frame twice.
	float cycle = (rewind ? 2.f * lastFrame : frames) + delay;

	// If this is the very first step, fill in some values that we could not set
	// until we knew the sprite's frame count and the starting step.
	if(randomize)
	{
		randomize = false;
		// The random offset can be a fractional frame.
		frameOffset += static_cast<float>(Random::Real()) * cycle;
	}
	else if(startAtZero)
	{
		startAtZero = false;
		// Adjust frameOffset so that this step's frame is exactly 0 (no fade).
		frameOffset -= frameRate * step;
	}

	// Figure out what fraction of the way in between frames we are. Avoid any
	// possible floating-point glitches that might result in a negative frame.
	frame = max(0.f, frameRate * step + frameOffset);
	// If repeating, wrap the frame index by the total cycle time.
	if(repeat)
		frame = fmod(frame, cycle);

	if(!rewind)
	{
		// If not repeating, frame should never go higher than the index of the
		// final frame.
		if(!repeat)
			frame = min(frame, lastFrame);
		else if(frame >= frames)
		{
			// If we're in the delay portion of the loop, set the frame to 0.
			frame = 0.f;
		}
	}
	else if(frame >= lastFrame)
	{
		// In rewind mode, once you get to the last frame, count backwards.
		// Regardless of whether we're repeating, if the frame count gets to
		// be less than 0, clamp it to 0.
		frame = max(0.f, lastFrame * 2.f - frame);
	}
}
