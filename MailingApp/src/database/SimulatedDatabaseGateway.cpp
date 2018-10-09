#include "SimulatedDatabaseChatGateway.h"



SimulatedDatabaseChatGateway::SimulatedDatabaseChatGateway()
{
}


SimulatedDatabaseChatGateway::~SimulatedDatabaseChatGateway()
{
}

void SimulatedDatabaseChatGateway::insertMessage(const MessageChat & message)
{
	allMessages.push_back(message);
}

void SimulatedDatabaseChatGateway::ClearMessage(MessageChat & message)
{
	std::vector<MessageChat>::iterator item = allMessages.begin();
	while (item._Ptr != nullptr)
	{
		if (item._Ptr->body == message.body && 
			item._Ptr->senderUsername == message.senderUsername && 
			item._Ptr->date == message.date)
		{
			item = allMessages.erase(item);
			return;
		}
		else
		{
			item++;
		}

	}
}

std::vector<MessageChat> SimulatedDatabaseChatGateway::getAllMessages()
{
	return allMessages;
}
