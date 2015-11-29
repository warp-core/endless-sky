/* FileWriter.h
Copyright (c) 2014 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#ifndef FILE_WRITER_H_
#define FILE_WRITER_H_

#include "DataWriter.h"



// Class for writing DataFile data to a file, replaces the old DataWriter
class FileWriter : public DataWriter {
public:
    FileWriter(const std::string &path);
   ~FileWriter();
   
   void Write() override;
   using DataWriter::Write;
   
   void BeginChild() override;
   void EndChild() override;
   
   void WriteComment(const std::string &str) override;
   
protected:
    void AppendToken(const char *a) override;
   
private:
    std::string path;
    std::string indent;
    static const std::string space;
	const std::string *before;
	std::ostringstream out;
};



#endif
