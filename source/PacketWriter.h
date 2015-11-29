/* PacketWriter.h
Copyright (c) 2015 by Zach Anderson

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#ifndef PACKET_WRITER_H_
#define PACKET_WRITER_H_

#include "DataWriter.h"

#include <SDL2/SDL_net.h>



// Class for writing DataFile data over network packets, should be used as a DataWriter
class PacketWriter : public DataWriter {
public:
    PacketWriter(UDPpacket* &packet, char prefix);
    ~PacketWriter();
    
    void Write() override;
    using DataWriter::Write;
    
    void BeginChild() override;
    void EndChild() override;
    
    void WriteComment(const std::string &str) override;
    
protected:
    void AppendToken(const char *a) override;
    
private:
    UDPpacket *&packet;
    char prefix;
 	std::ostringstream out;
    bool needsDelineator = false;
    bool cueWrite = false;
 };



#endif
