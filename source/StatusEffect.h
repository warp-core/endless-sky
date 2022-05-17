/* StatusEffect.h
Copyright (c) 2022 by warp-core

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#ifndef STATUSEFFECT_H_
#define STATUSEFFECT_H_

#include <limits>

constexpr double MAX_DOUBLE = std::numeric_limits<double>::max;

Class StatusEffect {
public:
    void Damage(double incomingDamage);
    void ApplyEffect();
    void ApplyDecay();
    void CheckMin();
    void CheckMax();

private:
    string name;
    double decayCoefficient;
    double shieldPenetration;
    double effectCoefficient;
    string affected;
    StatusEffect *affected;

    double value;
    double max;
    double min;
    double protection;
    double resistance;

    string sparkVisual;


private:

}

#endif
