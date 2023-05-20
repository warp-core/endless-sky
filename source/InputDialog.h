/* InputDialog.h
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

#ifndef INPUT_DIALOG_H_
#define INPUT_DIALOG_H_

#include "Dialog.h"

#include <memory>

class InputField;



class InputDialog : public Dialog {
public:
	// A dialog can have request two different types of input: an integer, or a string.
	// In either case, an initial value to be present in the input field can also be given.
	template <class T>
	InputDialog(T *t, void (T::*fun)(int), const std::string &text,
		Truncate truncate = Truncate::NONE, bool allowsFastForward = false);
	template <class T>
	InputDialog(T *t, void (T::*fun)(int), const std::string &text, int initialValue,
		Truncate truncate = Truncate::NONE, bool allowsFastForward = false);

	template <class T>
	InputDialog(T *t, void (T::*fun)(const std::string &), const std::string &text, std::string initialValue = "",
		Truncate truncate = Truncate::NONE, bool allowsFastForward = false);
	// This callback requests text input but with validation. The "ok" button is disabled
	// if the validation callback returns false.
	template <class T>
	InputDialog(T *t, void (T::*fun)(const std::string &), const std::string &text,
			std::function<bool(const std::string &)> validate,
			std::string initialValue = "",
			Truncate truncate = Truncate::NONE,
			bool allowsFastForward = false);

	// Draw this panel.
	virtual void Draw() override;


protected:
	// The user can click "ok" or "cancel", or use the tab key to toggle which
	// button is highlighted and the enter key to select it.
	virtual bool KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress) override;

	virtual void FinishInit() override;


private:
	void CreateIntField();
	void CreateStringField();

	void DoCallback();


protected:
	std::string input;


private:
	std::shared_ptr<InputField> inputField = nullptr;

	std::function<void(const std::string &)> stringFun;
	std::function<bool(const std::string &)> validateFun;
};



template <class T>
InputDialog::InputDialog(T *t, void (T::*fun)(int), const std::string &text, Truncate truncate, bool allowsFastForward)
	: Dialog(t, fun)
{
	this->allowsFastForward = allowsFastForward;
	Init(text, truncate);
	CreateIntField();
}



template <class T>
InputDialog::InputDialog(T *t, void (T::*fun)(int), const std::string &text,
	int initialValue, Truncate truncate, bool allowsFastForward)
	: Dialog(t, fun), input(std::to_string(initialValue))
{
	this->allowsFastForward = allowsFastForward;
	Init(text, truncate);
	CreateIntField();
}



template <class T>
InputDialog::InputDialog(T *t, void (T::*fun)(const std::string &), const std::string &text,
	std::string initialValue, Truncate truncate, bool allowsFastForward)
	: input(initialValue),
	stringFun(std::bind(fun, t, std::placeholders::_1))
{
	this->allowsFastForward = allowsFastForward;
	Init(text, truncate);
	CreateStringField();
}



template <class T>
InputDialog::InputDialog(T *t, void (T::*fun)(const std::string &), const std::string &text,
	std::function<bool(const std::string &)> validate, std::string initialValue, Truncate truncate, bool allowsFastForward)
	: input(initialValue),
	stringFun(std::bind(fun, t, std::placeholders::_1)),
	validateFun(std::move(validate))
{
	this->allowsFastForward = allowsFastForward;
	isOkDisabled = true;
	Init(text, truncate);
	CreateStringField();
}



#endif
