/* GameLoadingPanel.h
Copyright (c) 2021 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#ifndef GAME_LOADING_PANEL_H_
#define GAME_LOADING_PANEL_H_

#include "Panel.h"

#include <functional>
#include <string>
#include <vector>

class Editor;



// Class representing the loading menu, which is shown when loading resources
// (like game data and save files).
class GameLoadingPanel final : public Panel {
public:
	GameLoadingPanel(std::function<void(GameLoadingPanel *)> done, bool &finishedLoading);

	void Step() final;
	void Draw() final;


private:
	std::function<void(GameLoadingPanel *)> done;
	bool &finishedLoading;

	// The circular loading indicator shows 60 tick marks when all game data is loaded.
	const int MAX_TICKS = 60;
	const double ANGLE_OFFSET;
	// The current number of ticks to be displayed.
	int progress = 0;
};



#endif
