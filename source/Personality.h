/* Personality.h
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

#ifndef PERSONALITY_H_
#define PERSONALITY_H_

#include "Angle.h"
#include "Point.h"

#include <bitset>

class DataNode;
class DataWriter;



// Class defining an AI "personality": what actions it takes, and how skilled
// and aggressive it is in combat. This also includes some more specialized
// behaviors, like plundering ships or launching surveillance drones, that are
// used to make some fleets noticeably different from others.
class Personality {
public :
	static constexpr auto PACIFIST = 0;
	static constexpr auto FORBEARING = 1;
	static constexpr auto TIMID = 2;
	static constexpr auto DISABLES = 3;
	static constexpr auto PLUNDERS = 4;
	static constexpr auto HUNTING = 5;
	static constexpr auto STAYING = 6;
	static constexpr auto ENTERING = 7;
	static constexpr auto NEMESIS = 8;
	static constexpr auto SURVEILLANCE = 9;
	static constexpr auto UNINTERESTED = 10;
	static constexpr auto WAITING = 11;
	static constexpr auto DERELICT = 12;
	static constexpr auto FLEEING = 13;
	static constexpr auto ESCORT = 14;
	static constexpr auto FRUGAL = 15;
	static constexpr auto COWARD = 16;
	static constexpr auto VINDICTIVE = 17;
	static constexpr auto SWARMING = 18;
	static constexpr auto UNCONSTRAINED = 19;
	static constexpr auto MINING = 20;
	static constexpr auto HARVESTS = 21;
	static constexpr auto APPEASING = 22;
	static constexpr auto MUTE = 23;
	static constexpr auto OPPORTUNISTIC = 24;
	static constexpr auto MERCIFUL = 25;
	static constexpr auto TARGET = 26;
	static constexpr auto MARKED = 27;
	static constexpr auto LAUNCHING = 28;
	static constexpr auto LINGERING = 29;
	static constexpr auto DARING = 30;
	static constexpr auto SECRETIVE = 31;
	static constexpr auto RAMMING = 32;
	static constexpr auto DECLOAKED = 33;

public:
	Personality() noexcept;
	Personality(int personality) noexcept;

	void Load(const DataNode &node);
	void Save(DataWriter &out) const;

	bool IsDefined() const;

	// Who a ship decides to attack:
	bool IsPacifist() const;
	bool IsForbearing() const;
	bool IsTimid() const;
	bool IsHunting() const;
	bool IsNemesis() const;
	bool IsDaring() const;

	// How they fight:
	bool IsFrugal() const;
	bool Disables() const;
	bool Plunders() const;
	bool IsVindictive() const;
	bool IsUnconstrained() const;
	bool IsCoward() const;
	bool IsAppeasing() const;
	bool IsOpportunistic() const;
	bool IsMerciful() const;
	bool IsRamming() const;

	// Mission NPC states:
	bool IsStaying() const;
	bool IsEntering() const;
	bool IsWaiting() const;
	bool IsLaunching() const;
	bool IsFleeing() const;
	bool IsDerelict() const;
	bool IsUninterested() const;

	// Non-combat goals:
	bool IsSurveillance() const;
	bool IsMining() const;
	bool Harvests() const;
	bool IsSwarming() const;
	bool IsLingering() const;
	bool IsSecretive() const;

	// Special flags:
	bool IsEscort() const;
	bool IsTarget() const;
	bool IsMarked() const;
	bool IsMute() const;
	bool IsDecloaked() const;

	// Current inaccuracy in this ship's targeting:
	const Point &Confusion() const;
	void UpdateConfusion(bool isFiring);

	// Personality to use for ships defending a planet from domination:
	static Personality Defender();
	static Personality DefenderFighter();


private:
	void Parse(const DataNode &node, int index, bool remove);


private:
	// Make sure this matches the number of items in PersonalityTrait,
	// or the build will fail.
	static const int PERSONALITY_COUNT = 34;

	bool isDefined = false;

	std::bitset<PERSONALITY_COUNT> flags;
	double confusionMultiplier;
	double aimMultiplier;
	Point confusion;
	Point confusionVelocity;

	friend class FleetEditor;
};



#endif
