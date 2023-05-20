/* InputField.h
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

#ifndef INPUT_FIELD_H_
#define INPUT_FIELD_H_

#include "Panel.h"

#include "text/DisplayText.h"
#include "InputType.h"

#include <memory>



// Class representing an input field where the user can type a string.
class InputField : public Panel, public std::enable_shared_from_this<InputField> {
public:
	InputField(const Rectangle &rect, InputType type, std::string initialValue = "");

	// Move the state of this panel forward one game step.
	virtual void Step() override;

	// Draw this panel.
	virtual void Draw() override;

	void SetIsActive(bool state);
	void SetValidationFunction(std::function<bool(std::string, char)> fun);

	const std::string &GetContent() const;


protected:
	virtual bool KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress) override;


private:
	void UpdateDisplayText();


private:
	InputType type = InputType::STRING;

	Rectangle region;

	std::string content;
	DisplayText text;

	bool isActive = true;

	int flickerTime = 0;
	std::function<bool(std::string, char)> validator = nullptr;
};



#endif
