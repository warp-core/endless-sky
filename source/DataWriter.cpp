/* DataWriter.h
Copyright (c) 2014 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "DataWriter.h"

#include "DataNode.h"
#include "Files.h"

using namespace std;



void DataWriter::Write(const DataNode &node)
{
	for(int i = 0; i < node.Size(); ++i)
		WriteToken(node.Token(i).c_str());
	Write();
	
	if(node.begin() != node.end())
	{
		BeginChild();
		{
			for(const DataNode &child : node)
				Write(child);
		}
		EndChild();
	}
}



void DataWriter::WriteToken(const char *a)
{
	bool hasSpace = !*a;
	bool hasQuote = false;
	for(const char *it = a; *it; ++it)
	{
		hasSpace |= (*it <= ' ');
		hasQuote |= (*it == '"');
	}
	
	ostringstream out;
	if(hasSpace && hasQuote)
		out << '`' << a << '`';
	else if(hasSpace)
		out << '"' << a << '"';
	else
		out << a;
	AppendToken(out.str().c_str());
}
