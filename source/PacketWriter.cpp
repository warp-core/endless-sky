/* PacketWriter.cpp
Copyright (c) 2015 by Zach Anderson

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "PacketWriter.h"

using namespace std;



PacketWriter::PacketWriter(char prefix)
{
	out << string(1, prefix);
}


PacketWriter::PacketWriter(string prefix)
{
	out << prefix;
}



UDPpacket *PacketWriter::Flush()
{
    const string str =  out.str();
    const unsigned int length = str.size();
    const char *string = str.c_str();
    
    UDPpacket *packet = SDLNet_AllocPacket(length);
	
	packet->len = length;
    
    for(unsigned int i = 0; i < length; i++) {
        packet->data[i] = string[i];
    }

	return packet;
}



void PacketWriter::Write()
{
    cueWrite = true;
    needsDelineator = false;
}



void PacketWriter::BeginChild()
{
    out << '\x1C';
    needsDelineator = false;
    cueWrite = false;
}



void PacketWriter::EndChild()
{
    out << '\x1D';
    needsDelineator = false;
    cueWrite = false;
}



void PacketWriter::WriteComment(const string &str)
{
    // Do nothing, we ignore comments when writing over network
}



void PacketWriter::WriteToken(const char *a)
{
    if(cueWrite)
        out << '\x1E';
    if(needsDelineator)
        out << '\x1F';
    out << a;
    
    cueWrite = false;
    needsDelineator = true;
}
