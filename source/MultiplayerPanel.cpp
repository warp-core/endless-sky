/* MultiplayerPanel.cpp
Copyright (c) 2015 by Zach Anderson

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "MultiplayerPanel.h"

#include "Dialog.h"

#include "Format.h"
#include "GameData.h"
#include "Information.h"
#include "Interface.h"
#include "ListenServer.h"
#include "LoadPanel.h"
#include "PlayerInfo.h"
#include "Point.h"
#include "Server.h"
#include "ServerInfo.h"
#include "StarField.h"
#include "Table.h"
#include "UI.h"

#include <iostream>

using namespace std;



MultiplayerPanel::MultiplayerPanel(PlayerInfo &player, UI &gamePanels, ListenServer*& server)
	: player(player), gamePanels(gamePanels), serverPointer(server)
{
	servers.reserve(10);
	Server::LoadServerList(servers);
}



void MultiplayerPanel::Draw() const
{
	glClear(GL_COLOR_BUFFER_BIT);
	GameData::Background().Draw(Point(), Point());
	
	Information info;
	
	const Interface *menu = GameData::Interfaces().Get("multiplayer");
	menu->Draw(info);
	
    Color back   = *GameData::Colors().Get("faint");
	Color dim    = *GameData::Colors().Get("dim");
	Color medium = *GameData::Colors().Get("medium");
	Color bright = *GameData::Colors().Get("bright");
	
	Table table;
	table.AddColumn(-115, Table::LEFT);
	table.AddColumn(115, Table::RIGHT);
	
    table.SetHighlight(-115,115);
    table.SetUnderline(-115,115);
	
	int firstY = -167;
	table.DrawAt(Point(0, firstY));
	
	table.DrawUnderline();
    table.Draw("Servers", bright);
	table.Draw("Ping"   , bright);
	
    zones.clear();
	
    for(unsigned int i = 0; i < servers.size(); i++)
	{
		ServerInfo info = servers[i].GetStatus();
        zones.emplace_back(table.GetCenterPoint(), table.GetRowSize(), info.name);
		
        if(i == selected)
            table.DrawHighlight(dim);
        else if(i == hovered)
            table.DrawHighlight(back);
        
		if(info.alive)
		{
	        table.Draw(info.name, medium);
			table.Draw(info.ping, medium);
		}
		else
		{
			table.Draw(info.name, dim);
			table.Draw("Offline", dim);
		}
    }
}



bool MultiplayerPanel::KeyDown(SDL_Keycode key, Uint16 mod, const Command &command)
{
	if(key == 'b')
	{
		Server::SaveServerList(servers);
		GetUI()->Pop(this);
	}
	else if(key == 'j')
	{
		// Make sure there is a server selected first
		if(selected != -1u)
		{
        	cout << "Joining server " << zones[selected].Value() << endl;
			GetUI()->Push(new Dialog(this, &MultiplayerPanel::CancelConnection,
				"Connecting to " + zones[selected].Value() + ". Please wait... Press OK to Cancel"));
			// TODO Actually connect
		}
	}
    else if(key == 'c')
	{
		GetUI()->Push(new Dialog(this, &MultiplayerPanel::CreateServer,
			"Enter Server Name:"));
	}
	else if(key == 'a')
	{
		GetUI()->Push(new Dialog(this, &MultiplayerPanel::AddServer,
			"Enter IP Address:"));
	}
	else
	{
		cout << "Unhandled key <" << key << ">" << endl;
		return false;
	}
	return true;
}



void MultiplayerPanel::CancelConnection() {
	cout << "Connection attempt canceled" << endl;
}



void MultiplayerPanel::CreateServer(const string &value) {
	cout << "Created new server: " << value << endl;
	serverPointer = new ListenServer(value);
	GetUI()->Pop(this);
	GetUI()->Push(new LoadPanel(player, gamePanels));
}



void MultiplayerPanel::AddServer(const string &value) {
	servers.emplace_back(value, 7895);
}



bool MultiplayerPanel::Click(int x, int y)
{
    Point point(x, y);
	
	char key = GameData::Interfaces().Get("multiplayer")->OnClick(Point(x, y));
	if(key)
		return DoKey(key);
	
    for(unsigned index = 0; index < zones.size(); ++index)
		if(zones[index].Contains(point))
			selected = index;
	
	return true;
}



bool MultiplayerPanel::Hover(int x, int y)
{
    Point point(x, y);
	hovered = -1u;
    for(unsigned index = 0; index < zones.size(); ++index)
		if(zones[index].Contains(point))
			hovered = index;
	
	return true;
}
