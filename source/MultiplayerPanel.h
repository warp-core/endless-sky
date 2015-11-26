/* MultiplayerPanel.h
Copyright (c) 2015 by Zach Anderson

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#ifndef MULTIPLAYER_PANEL_H_
#define MULTIPLAYER_PANEL_H_

#include "Panel.h"

#include "ClickZone.h"

#include <string>
#include <vector>

class ListenServer;
class PlayerInfo;
class Server;



// UI panel for managing and connecting to servers
class MultiplayerPanel : public Panel {
public:
	MultiplayerPanel(PlayerInfo &player, UI &mainUI, ListenServer*& server);

	virtual void Draw() const override;
	
	void AddServer(const std::string &value);
	void CreateServer(const std::string &value);
	void CancelConnection();


protected:
	virtual bool KeyDown(SDL_Keycode key, Uint16 mod, const Command &command) override;
	virtual bool Click(int x, int y) override;
	virtual bool Hover(int x, int y) override;

private:
	PlayerInfo &player;
	UI &gamePanels;
	ListenServer *&serverPointer;
	
    unsigned int hovered = -1u;
    unsigned int selected = -1u;
	
	mutable std::vector<ClickZone<std::string>> zones;
	std::vector<Server> servers;
};



#endif
