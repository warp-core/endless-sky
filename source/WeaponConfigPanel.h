/*WeaponConfigPanel.h
Copyright (c) 2022 by warpcore

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#ifndef WEAPONCONFIGPANEL_H_
#define WEAPONCONFIGPANEL_H_

#include "Panel.h"

#include "ClickZone.h"
#include "InfoPanelState.h"
#include "Point.h"
#include "text/layout.hpp"

#include <vector>

class Color;
class PlayerInfo;

class WeaponConfigPanel : public Panel {
public:
    explicit WeaponConfigPanel(PlayerInfo &player);
	explicit WeaponConfigPanel(PlayerInfo &player, InfoPanelState state);

	virtual void Step() override;
	virtual void Draw() override;


protected:
	// Only override the ones you need; the default action is to return false.
	virtual bool KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress);
	virtual bool Click(int x, int y, int clicks);
	virtual bool RClick(int x, int y);
	virtual bool Hover(int x, int y);
	virtual bool Drag(double dx, double dy);
	virtual bool Release(int x, int y);
	virtual bool Scroll(double dx, double dy);


private:
    // Handle a change to what ship is shown.
	void UpdateInfo();
	void ClearZones();

	void DrawWeapons(const Rectangle &silhouetteBounds, const Rectangle &weaponsBounds);

	// Helper functions.
	void DrawLine(const Point &from, const Point &to, const Color &color) const;
	bool Hover(const Point &point);


private:
	PlayerInfo &player;
	// This is the currently selected ship.
	std::vector<std::shared_ptr<Ship>>::const_iterator shipIt;

	// Track all the clickable parts of the UI (other than the buttons).
	std::vector<ClickZone<int>> zones;
	// Keep track of which item the mouse is hovering over and which item is
	// currently being dragged.
	int hoverIndex = -1;
	int draggingIndex = -1;

	InfoPanelState panelState;

	// Track the current mouse location.
	Point hoverPoint;
};

#endif