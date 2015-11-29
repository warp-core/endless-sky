/* FileWriter.cpp
Copyright (c) 2014 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "FileWriter.h"

#include "Files.h"

using namespace std;



const string FileWriter::space = " ";



FileWriter::FileWriter(const string &path)
	: path(path), before(&indent)
{
	out.precision(8);
}



FileWriter::~FileWriter()
{
	Files::Write(path, out.str());
}



void FileWriter::Write()
{
	out << '\n';
	before = &indent;
}



void FileWriter::BeginChild()
{
	indent += '\t';
}



void FileWriter::EndChild()
{
	indent.erase(indent.length() - 1);
}



void FileWriter::WriteComment(const string &str)
{
	out << indent << "# " << str << '\n';
}



void FileWriter::AppendToken(const char *a) {
    out << *before << a;
    before = &space;
}
