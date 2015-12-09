/* DataReader.h
Copyright (c) 2015 by Zach Anderson

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#ifndef DATA_READER_H_
#define DATA_READER_H_

#include <list>

class DataNode;



// Abstract superclass to allow a `PacketReader` and `DataFile` to be used interchangeably.
class DataReader {
public:
	virtual std::list<DataNode>::const_iterator begin() const = 0;
	virtual std::list<DataNode>::const_iterator end() const = 0;
};



#endif
