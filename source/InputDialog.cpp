/* InputDialog.cpp
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

#include "InputDialog.h"

#include "Color.h"
#include "FillShader.h"
#include "text/Font.h"
#include "text/FontSet.h"
#include "GameData.h"
#include "InputField.h"
#include "shift.h"
#include "Sprite.h"
#include "SpriteSet.h"
#include "UI.h"

using namespace std;

namespace {
	const int WIDTH = 250;

	Rectangle FindInputPos(int height)
	{
		// We need to konw the dimensions of these sprites to determine where to
		// place the input field.
		const Sprite *top = SpriteSet::Get("ui/dialog top");
		const Sprite *middle = SpriteSet::Get("ui/dialog middle");
		const Sprite *bottom = SpriteSet::Get("ui/dialog bottom");

		const double inputY = (top->Height() + height * middle->Height() + bottom->Height()) * .5f - 70.;
		const Point inputDim(WIDTH - 20., 20.);
		const Point inputCenter = Point(0., inputY) + inputDim / 2.;

		return Rectangle(inputCenter, inputDim);
	}

	// Map any conceivable numeric keypad keys to their ASCII values. Most of
	// these will presumably only exist on special programming keyboards.
	const map<SDL_Keycode, char> KEY_MAP = {
		{SDLK_KP_0, '0'},
		{SDLK_KP_1, '1'},
		{SDLK_KP_2, '2'},
		{SDLK_KP_3, '3'},
		{SDLK_KP_4, '4'},
		{SDLK_KP_5, '5'},
		{SDLK_KP_6, '6'},
		{SDLK_KP_7, '7'},
		{SDLK_KP_8, '8'},
		{SDLK_KP_9, '9'},
		{SDLK_KP_AMPERSAND, '&'},
		{SDLK_KP_AT, '@'},
		{SDLK_KP_A, 'a'},
		{SDLK_KP_B, 'b'},
		{SDLK_KP_C, 'c'},
		{SDLK_KP_D, 'd'},
		{SDLK_KP_E, 'e'},
		{SDLK_KP_F, 'f'},
		{SDLK_KP_COLON, ':'},
		{SDLK_KP_COMMA, ','},
		{SDLK_KP_DIVIDE, '/'},
		{SDLK_KP_EQUALS, '='},
		{SDLK_KP_EXCLAM, '!'},
		{SDLK_KP_GREATER, '>'},
		{SDLK_KP_HASH, '#'},
		{SDLK_KP_LEFTBRACE, '{'},
		{SDLK_KP_LEFTPAREN, '('},
		{SDLK_KP_LESS, '<'},
		{SDLK_KP_MINUS, '-'},
		{SDLK_KP_MULTIPLY, '*'},
		{SDLK_KP_PERCENT, '%'},
		{SDLK_KP_PERIOD, '.'},
		{SDLK_KP_PLUS, '+'},
		{SDLK_KP_POWER, '^'},
		{SDLK_KP_RIGHTBRACE, '}'},
		{SDLK_KP_RIGHTPAREN, ')'},
		{SDLK_KP_SPACE, ' '},
		{SDLK_KP_VERTICALBAR, '|'}
	};
}



void InputDialog::Draw()
{
	// Draw the dialog box.
	Dialog::Draw();
	// Nothing else needs to be done here.
	// The InputField is on the UI stack and will draw itself.
}



bool InputDialog::KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress)
{
	bool isCloseRequest = key == SDLK_ESCAPE || (key == 'w' && (mod & (KMOD_CTRL | KMOD_GUI)));
	if(key == SDLK_RETURN || key == SDLK_KP_ENTER || isCloseRequest)
	{
		if(okIsActive)
		{
			// If the OK button is disabled (because the input failed the validation),
			// don't execute the callback.
			if(!isOkDisabled)
			{
				DoCallback();
				GetUI()->Pop(inputField.get());
				GetUI()->Pop(this);
			}
		}
		else
		{
			GetUI()->Pop(inputField.get());
			GetUI()->Pop(this);
		}
	}
	else
		return false;

	return true;
}



void InputDialog::FinishInit()
{
	height += 30 * (!isMission && (intFun || stringFun));
	Dialog::FinishInit();
}



void InputDialog::CreateIntField()
{
	const Rectangle inputArea = FindInputPos(height);
	inputField = shared_ptr<InputField>(new InputField(inputArea, InputType::NUMBER, input));
	GetUI()->Push(inputField.get()->shared_from_this());
}



void InputDialog::CreateStringField()
{
	const Rectangle inputArea = FindInputPos(height);
	inputField = shared_ptr<InputField>(new InputField(inputArea, InputType::STRING, input));
	GetUI()->Push(inputField.get()->shared_from_this());
	//GetUI()->Push(new InputField(inputArea, InputType::STRING));
	//inputField = static_cast<InputField *>(GetUI()->Top().get())->shared_from_this();
}



void InputDialog::DoCallback()
{
	input = inputField->GetContent();

	if(intFun)
	{
		// Only call the callback if the input can be converted to an int.
		// Otherwise treat this as if the player clicked "cancel."
		try {
			intFun(stoi(input));
		}
		catch(...)
		{
		}
	}

	if(stringFun)
		stringFun(input);
}
