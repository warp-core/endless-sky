/* ConversationInstance.h
Copyright (c) 2022 by warp-core

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#ifndef CONVERSATION_INSTANCE_H_
#define CONVERSATION_INSTANCE_H_

#include "Conversation.h"



class ConversationInstance : public Conversation {
public:
	ConversationInstance(const Conversation &conversation);

};

#endif
