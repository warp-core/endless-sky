/* CheckTextLength.cpp
Copyright (c) 2022 by warp-core

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/



#include "CheckTextLength.h"

#include "alignment.hpp"
#include "FontSet.h"
#include "Format.h"
#include "WrappedText.h"

using namespace std;

namespace {
	const map<string, string> GENERIC_SUBS = {
		{"<bunks>", "[N]"},
		{"<cargo>", "[N tons of Commodity]"},
		{"<commodity>", "[Commodity]"},
		{"<date>", "[Day Mon Year]"},
		{"<day>", "[The Nth of Month]"},
		{"<destination>", "[Planet in the Star system]"},
		{"<fare>", "[N passengers]"},
		{"<first>", "[First]"},
		{"<last>", "[Last]"},
		{"<origin>", "[Origin Planet]"},
		{"<passengers>", "[your passengers]"},
		{"<planet>", "[Planet]"},
		{"<ship>", "[Ship]"},
		{"<system>", "[Star]"},
		{"<tons>", "[N tons]"}
	};
}



bool CheckTextLength::CheckLength(const string &text, int width, int lines)
{
	if(text.empty())
		return false;
	WrappedText wrap;
	wrap.SetFont(FontSet::Get(14));
	wrap.SetAlignment(Alignment::JUSTIFIED);
	wrap.SetWrapWidth(width);
	wrap.Wrap(Format::Replace(text, GENERIC_SUBS));
	int lineHeight = wrap.LineHeight();
	if(!lineHeight)
		return false;
	double lineCount = wrap.Height() / lineHeight;
	return lineCount > lines;
}
