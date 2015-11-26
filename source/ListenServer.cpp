/* ListenServer.cpp
Copyright (c) 2015 by Zach Anderson

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include "ListenServer.h"

#include <iostream>

using namespace std;



ListenServer::ListenServer(std::string name)
    :name(name)
{
    Init(7895);
}



void ListenServer::Init(int port)
{
    socket = SDLNet_UDP_Open(7895);
    if(!socket) {
        printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        exit(2);
    }
    printf("Opened socket\n");
}



void ListenServer::Step()
{
    UDPpacket *packet = SDLNet_AllocPacket(8);
    int numrecv = SDLNet_UDP_Recv(socket, packet);
    if(numrecv)
    {
        switch(*(packet->data))
        {
            // Echo the packet for a ping
            case 'P':
                SDLNet_UDP_Send(socket, -1, packet);
                break;
            
            case 'Q':
            {
                Uint8 nameLength= name.size();
                // Prepare the data
                Uint8* buffer = new Uint8[7 + nameLength];
                buffer[0] = 'R';

                buffer[1] = nameLength;
                for(int i = 0; i < nameLength; i++)
                    buffer[i+2]=name[i];
                
                
                buffer[2+nameLength] = ingame;
                buffer[2+nameLength+1] = maxPlayers;
                buffer[2+nameLength+2] = hasPassword;
                
                // Prepare the packet
                UDPpacket* echo = new UDPpacket();
                echo->channel = -1;
                echo->data = buffer;
                echo->len = 7 + nameLength;
                echo->maxlen = 7 + nameLength;
                echo->address = packet->address;
                
                // Send the packet
                int numsent=SDLNet_UDP_Send(socket, -1, echo);
                if(!numsent)
                    printf("SDLNet_UDP_Send: %s\n", SDLNet_GetError());
                break;
            }
            
            default:
                cout << "Malformed packet" << endl;
                break;
        }
    }
    SDLNet_FreePacket(packet);
}
