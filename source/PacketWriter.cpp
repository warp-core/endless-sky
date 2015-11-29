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



PacketWriter::PacketWriter(UDPpacket* &packet, char prefix)
    :packet(packet), prefix(prefix)
{
    
}



PacketWriter::~PacketWriter()
{
    const string str =  out.str();
    const unsigned int length = str.size() + 1;
    const char *string = str.c_str();
    
    packet = SDLNet_AllocPacket(length);
    packet->len = length;
    packet->data = new unsigned char[length];
    
    packet->data[0] = prefix;
    for(unsigned int i = 1; i < length; i++) {
        packet->data[i] = string[i - 1];
    }
}



void PacketWriter::Write()
{
    cueWrite = true;
    needsDelineator = false;
}



void PacketWriter::BeginChild()
{
    out << '(';
    needsDelineator = false;
    cueWrite = false;
}



void PacketWriter::EndChild()
{
    out << ')';
    needsDelineator = false;
    cueWrite = false;
}



void PacketWriter::WriteComment(const string &str)
{
    // Do nothing, we ignore comments when writing over network
}



void PacketWriter::AppendToken(const char *a)
{
    if(cueWrite)
        out << '|';
    if(needsDelineator)
        out << ':';
    out << a;
    
    cueWrite = false;
    needsDelineator = true;
}
