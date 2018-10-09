#pragma once

#include "DataBaseChat.h"
#include <vector>

class IDatabaseChatGateway
{
public:

	// Constructor and destructor

	IDatabaseChatGateway() { }
	
	virtual ~IDatabaseChatGateway() { }


	// Virtual methods

	virtual void insertMessage(const MessageChat &message) = 0;
	virtual void ClearMessage(MessageChat &message) = 0;

	virtual std::vector<MessageChat> getAllMessages() = 0;

	virtual void updateGUI() { }
};

