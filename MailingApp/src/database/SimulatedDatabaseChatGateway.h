#pragma once

#include "IDatabaseChatGateway.h"
#include <vector>

class SimulatedDatabaseChatGateway :
	public IDatabaseChatGateway
{
public:

	// Constructor and destructor

	SimulatedDatabaseChatGateway();

	~SimulatedDatabaseChatGateway();


	// Virtual methods from IDatabaseGateway

	void insertMessage(const MessageChat &message) override;

	void ClearMessage(MessageChat & message) override;

	std::vector<MessageChat> getAllMessages() override;

private:

	std::vector<MessageChat> allMessages;
};
