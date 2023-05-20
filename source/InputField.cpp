/* InputField.cpp
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

#include "InputField.h"

#include "Color.h"
#include "FillShader.h"
#include "text/Font.h"
#include "text/FontSet.h"
#include "GameData.h"
#include "Preferences.h"
#include "shift.h"

using namespace std;

namespace {
	int FONT_SIZE = 14;

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



InputField::InputField(const Rectangle &rect, InputType type, string initialValue)
	: type(type), region(rect), content(initialValue)
{
	SetTrapAllEvents(false);
	UpdateDisplayText();
}



// Move the state of this panel forward one game step.
void InputField::Step()
{
	if(flickerTime)
		--flickerTime;
}



// Draw this panel.
void InputField::Draw()
{
	const Color &faint = *GameData::Colors().Get("faint");
	const Color &dim = *GameData::Colors().Get("dim");
	const Color &medium = *GameData::Colors().Get("medium");
	const Color &bright = *GameData::Colors().Get("bright");

	// Draw the input area.
	const Color &areaColor = (flickerTime % 6 > 3 ? faint : medium);
	FillShader::Fill(region.Center(), region.Dimensions(), areaColor);

	// Draw the current content of the input field.
	Point textPos(region.Left() + 5., region.Center().Y() - .5 * FONT_SIZE);

	const Font &font = FontSet::Get(FONT_SIZE);
	font.Draw(text, textPos, bright);

	// Draw the text cursor.
	Point barPos(textPos.X() + font.FormattedWidth(text) + 2., textPos.Y());
	FillShader::Fill(barPos, Point(1., 16.), dim);
}



void InputField::SetIsActive(bool state)
{
	isActive = state;
}



void InputField::SetValidationFunction(std::function<bool(std::string, char)> fun)
{
	validator = fun;
}



const string &InputField::GetContent() const
{
	return content;
}



bool InputField::KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress)
{
	if(!isActive)
		return false;

	const bool shift = mod & KMOD_SHIFT;
	const bool caps = mod & KMOD_CAPS;

	auto it = KEY_MAP.find(key);
	if(it != KEY_MAP.end() || (key >= ' ' && key <= '~'))
	{
		int ascii = (it != KEY_MAP.end()) ? it->second : key;
		char c = (shift ? SHIFT[ascii] : ascii);
		// Caps lock should shift letters, but not any other keys.
		if(caps && c >= 'a' && c <= 'z')
			c += 'A' - 'a';

		if(validator && !validator(content, c))
			flickerTime = 10;
		else if(type == InputType::STRING)
			content += c;
		// Integer input should not allow leading zeros.
		else if(c == '0' && !content.empty())
			content += c;
		else if(c >= '1' && c <= '9')
			content += c;
	}
	else if((key == SDLK_DELETE || key == SDLK_BACKSPACE))
	{
		if(content.empty())
			flickerTime = 10;
		else
			content.erase(content.back());
	}
	else
		return false;

	UpdateDisplayText();
	return true;
}



/*bool InputField::Hover(int x, int y)
{
	const Point point(x, y);
	if(!captureMouseAnywhere && !region.Contains(point))
		return false;

	hoverPoint = point;
	hovered = region.Contains(point);
	return hovered;
}



bool InputField::Scroll(double dx, double dy)
{
	if(!captureMouseAnywhere && !hovered)
		return false;

	const double change = (dy ? dy : dx);
	MoveCursor(change * Preferences::ScrollSpeed());
	return true;
}



void InputField::MoveCursor(int units)
{
	cursorPos += units;
	cursorPos = max(0, cursorPos);
	cursorPos = min(static_cast<int>(content.size()), cursorPos);
}*/



void InputField::UpdateDisplayText()
{
	const Layout layout(region.Width() - 10, Alignment::LEFT, Truncate::FRONT);
	text = DisplayText(content, layout);
}
