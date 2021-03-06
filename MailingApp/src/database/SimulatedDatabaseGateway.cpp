#include "SimulatedDatabaseGateway.h"



SimulatedDatabaseGateway::SimulatedDatabaseGateway()
{
}


SimulatedDatabaseGateway::~SimulatedDatabaseGateway()
{
}

void SimulatedDatabaseGateway::insertMessage(const Message & message)
{
	allMessages.push_back(message);
}

void SimulatedDatabaseGateway::insertMessageGame(const MessageGameCaputred & message)
{
}

void SimulatedDatabaseGateway::ClearMessage(Message & message)
{
	std::vector<Message>::iterator item = allMessages.begin();
	while (item._Ptr != nullptr)
	{
		if (item._Ptr->body == message.body && 
			item._Ptr->receiverUsername == message.receiverUsername && 
			item._Ptr->senderUsername == message.senderUsername && 
			item._Ptr->subject == message.subject)
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

std::vector<Message> SimulatedDatabaseGateway::getAllMessagesReceivedByUser(const std::string & username)
{
	std::vector<Message> messages;
	for (const auto & message : allMessages)
	{
		if (message.receiverUsername == username)
		{
			messages.push_back(message);
		}
	}
	return messages;
}
