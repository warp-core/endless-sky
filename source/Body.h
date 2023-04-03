/* Body.h
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

#ifndef BODY_H_
#define BODY_H_

#include "BodyTemplate.h"

#include "Angle.h"
#include "Point.h"

class Government;



// Class representing any object in the game that has a position, velocity, and
// facing direction and usually also has a sprite.
class Body : public BodyTemplate {
public:
	// Constructors.
	Body() = default;
	Body(const BodyTemplate &sprite, Point position, Point velocity = Point(), Angle facing = Angle(), double zoom = 1.);
	Body(const Sprite *sprite, Point position, Point velocity = Point(), Angle facing = Angle(), double zoom = 1.);
	Body(const Body &sprite, Point position, Point velocity = Point(), Angle facing = Angle(), double zoom = 1.);

	// Get the sprite frame and mask for the given time step.
	float GetFrame(int step = -1) const;
	const Mask &GetMask(int step = -1) const;

	// Positional attributes.
	const Point &Position() const;
	const Point &Velocity() const;
	const Angle &Facing() const;
	Point Unit() const;

	// Check if this object is marked for removal from the game.
	bool ShouldBeRemoved() const;

	// Store the government here too, so that collision detection that is based
	// on the Body class can figure out which objects will collide.
	const Government *GetGovernment() const;


protected:
	void PauseAnimation();
	// Mark this object to be removed from the game.
	void MarkForRemoval();
	// Mark that this object should not be removed (e.g. a launched fighter).
	void UnmarkForRemoval();


protected:
	// Basic positional attributes.
	Point position;
	Point velocity;
	Angle angle;

	// Government, for use in collision checks.
	const Government *government = nullptr;


private:
	// Set what animation step we're on. This affects future calls to GetMask()
	// and GetFrame().
	void SetStep(int step) const;


private:
	int pause = 0;

	// Record when this object is marked for removal from the game.
	bool shouldBeRemoved = false;

	// Cache the frame calculation so it doesn't have to be repeated if given
	// the same step over and over again.
	mutable int currentStep = -1;
	mutable float frame = 0.f;
};



#endif
