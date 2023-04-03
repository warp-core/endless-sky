/* BodyTemplate.h
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

#ifndef BODY_TEMPLATE_H_
#define BODY_TEMPLATE_H_

#include <cstdint>
#include <string>

class Body;
class DataNode;
class DataWriter;
class Mask;
class Sprite;



// Class representing any object in the game that has a position, velocity, and
// facing direction and usually also has a sprite.
class BodyTemplate {
public:
	// Constructors.
	BodyTemplate() = default;

	// Check that this Body has a sprite and that the sprite has at least one frame.
	bool HasSprite() const;
	// Access the underlying Sprite object.
	const Sprite *GetSprite() const;
	// Get the dimensions of the sprite.
	double Width() const;
	double Height() const;
	// Get the farthest a part of this sprite can be from its center.
	double Radius() const;
	// Which color swizzle should be applied to the sprite?
	int GetSwizzle() const;

	double Zoom() const;
	double Scale() const;

	// Sprite serialization.
	void LoadSprite(const DataNode &node);
	void SaveSprite(DataWriter &out, const std::string &tag = "sprite") const;
	// Set the sprite.
	void SetSprite(const Sprite *sprite);
	// Set the color swizzle.
	void SetSwizzle(int swizzle);


protected:
	BodyTemplate(const Sprite *sprite, const float zoom, const bool randomize);

	// Adjust the frame rate.
	void SetFrameRate(float framesPerSecond);
	void AddFrameRate(float framesPerSecond);


protected:
	// A zoom of 1 means the sprite should be drawn at half size. For objects
	// whose sprites should be full size, use zoom = 2.
	float zoom = 1.f;
	float scale = 1.f;


private:
	// Animation parameters.
	const Sprite *sprite = nullptr;
	// Allow objects based on this one to adjust their frame rate and swizzle.
	int swizzle = 0;

	float frameRate = 2.f / 60.f;
	int delay = 0;
	// The chosen frame will be (step * frameRate) + frameOffset.
	mutable float frameOffset = 0.f;
	mutable bool startAtZero = false;
	mutable bool randomize = false;
	bool repeat = true;
	bool rewind = false;

	friend class Body;
};



#endif
