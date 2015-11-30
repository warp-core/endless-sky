/* PacketReader.h
Copyright (c) 2015 by Zach Anderson

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#ifndef PACKET_READER_H_
#define PACKET_READER_H_

#include "DataNode.h"

#include <SDL2/SDL_net.h>

#include <list>



// Network packet analog for `DataFile`. Reads data from a UDPpacket into a DataNode.
class PacketReader {
public:
	PacketReader() = default;
	PacketReader(const UDPpacket &packet);
	
    void Read(const UDPpacket &packet);
	
	std::list<DataNode>::const_iterator begin() const;
	std::list<DataNode>::const_iterator end() const;
	
	
private:
	void Load(const char *it, const char *end);
	
	
private:
	DataNode root;
};



#endif
