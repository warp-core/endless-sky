/* PacketReader.cpp
Copyright (c) 2015 by Zach Anderson

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "PacketReader.h"

#include <string>

using namespace std;



PacketReader::PacketReader(const UDPpacket &packet)
{
    Read(packet);
}



void PacketReader::Read(const UDPpacket &packet)
{
    // Cast is safe because we are only using ascii chars, therefore a signed
    // and unsiged char have the same value
    
    // The first byte is always the datatype, so skip it
    const char *start = reinterpret_cast<char *>(packet.data + 1);
    const char *end   = reinterpret_cast<char *>(packet.data + packet.len - 1);

	Load(start, end);
}



list<DataNode>::const_iterator PacketReader::begin() const
{
	return root.begin();
}



list<DataNode>::const_iterator PacketReader::end() const
{
	return root.end();
}



// Structure Codes:
// 0x1C -- Start of Child
// 0x1D -- End of Child
// 0x1E -- Seperates Lines
// 0x1F -- Seperates Tokens
void PacketReader::Load(const char *it, const char *end)
{
	vector<DataNode *> parents(1, &root);
	DataNode *lastAdded = &root;
	
	while(it <= end)
	{
        if(*it == '\x1C')
        {
            parents.push_back(lastAdded);
            it++;
        }
        
        if(*it == '\x1D')
        {
            parents.pop_back();
            it++;
        }
	    
		// Add this node as a child of the most recent parent
		list<DataNode> &children = parents.back()->children;
		children.push_back(DataNode());
		DataNode &node = children.back();
	    
		lastAdded = &node;
	    
		// Tokenize the line (Lines can be delineated by any of those three chars)
		while(it <= end && *it != '\x1C' && *it != '\x1D' && *it != '\x1E')
		{
			const char *start = it;
	        
			// Find the end of this token. All structure marks are control codes which are less than space
			while(*it >= ' ' && it <= end)
				it++;
			  
			// Add a node to hold the token
			if(start == it)
				node.tokens.emplace_back();
			else
				node.tokens.emplace_back(start, it);
                        
            // Advance past the delineator for the token
            if(*it == '\x1F')
				it++;
		}
        
        // If the last delineator was a start or end child, we need to process
        // it at the start of next loop, otherwise move on to the next char
        if(*it != '\x1C' && *it != '\x1D')
            it++;
	}
}
