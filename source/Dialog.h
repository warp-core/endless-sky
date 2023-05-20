/* Dialog.h
Copyright (c) 2014-2020 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef DIALOG_H_
#define DIALOG_H_

#include "Panel.h"

#include "Point.h"
#include "text/truncate.hpp"
#include "text/WrappedText.h"

#include <functional>
#include <string>

class DataNode;
class PlayerInfo;
class System;



// A dialog box displays a given message to the player. The box will expand to
// fit the message, and may also include a text input field. The box may have
// only an "ok" button, or may also have a "cancel" button. If this dialog is
// introducing a mission, the buttons are instead "accept" and "decline". A
// callback function can be given to receive the player's response.
class Dialog : public Panel {
public:
	// Dialog that has no callback (information only). In this form, there is
	// only an "ok" button, not a "cancel" button.
	explicit Dialog(const std::string &text, Truncate truncate = Truncate::NONE, bool allowsFastForward = false);
	// Mission accept / decline dialog.
	Dialog(const std::string &text, PlayerInfo &player, const System *system = nullptr,
		Truncate truncate = Truncate::NONE, bool allowsFastForward = false);
	virtual ~Dialog() = default;

	// A dialog can be constructed with a callback method that it will call if
	// the user selects "ok", but not if they select "cancel."
	template <class T>
	Dialog(T *t, void (T::*fun)(), const std::string &text,
		Truncate truncate = Truncate::NONE, bool allowsFastForward = false);

	// Draw this panel.
	virtual void Draw() override;

	// Static method used to convert a DataNode into formatted Dialog text.
	static void ParseTextNode(const DataNode &node, size_t startingIndex, std::string &text);

	// Some dialogs allow fast-forward to stay active.
	bool AllowsFastForward() const noexcept final;


protected:
	// Constructor for use by InputDialog to initialise members of this class.
	Dialog() = default;
	template <class T>
	Dialog(T *t, void (T::*fun)(int));

	// The user can click "ok" or "cancel", or use the tab key to toggle which
	// button is highlighted and the enter key to select it.
	virtual bool KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress) override;
	virtual bool Click(int x, int y, int clicks) override;

	// Common code from all three constructors:
	void Init(const std::string &message, Truncate truncate, bool canCancel = true, bool isMission = false);

	virtual void FinishInit();

	void SetAllowsFastForward(bool state);


private:
	void DoCallback() const;


protected:
	WrappedText text;
	int height;

	// 'intFun' is a member of Dialog instead of InputDialog because it is used
	// to call Player::MissionCallback when offerin a mission, even when no user
	// input is required.
	std::function<void(int)> intFun;
	std::function<void()> voidFun;

	bool canCancel = true;
	bool okIsActive = true;
	bool isMission = false;
	bool isOkDisabled = false;
	bool allowsFastForward = false;

	Point okPos;
	Point cancelPos;

	const System *system = nullptr;
	PlayerInfo *player = nullptr;
};



template <class T>
Dialog::Dialog(T *t, void (T::*fun)(), const std::string &text, Truncate truncate, bool allowsFastForward)
	: voidFun(std::bind(fun, t)), allowsFastForward(allowsFastForward)
{
	Init(text, truncate);
}



template <class T>
Dialog::Dialog(T *t, void (T::*fun)(int))
	: intFun(std::bind(fun, t, std::placeholders::_1))
{
}



#endif
