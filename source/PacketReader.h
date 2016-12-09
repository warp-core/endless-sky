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

#include "DataReader.h"

#include "DataNode.h"

#include <SDL2/SDL_net.h>



// Network packet analog for `DataFile`. Reads data from a UDPpacket into a DataNode.
class PacketReader : public DataReader {
public:
	PacketReader() = default;
	PacketReader(const UDPpacket &packet, unsigned int burn = 1);
	
    void Read(const UDPpacket &packet);
    void Read(const UDPpacket &packet, unsigned int burn);
	
	std::list<DataNode>::const_iterator begin() const override;
	std::list<DataNode>::const_iterator end() const override;
	
	
private:
	void Load(const char *it, const char *end);
	
	
private:
	DataNode root;
};



#endif
