#pragma once

#include "IDatabaseGateway.h"

class MySqlDatabaseGateway :
	public IDatabaseGateway
{
public:

	// Constructor and destructor

	MySqlDatabaseGateway();

	~MySqlDatabaseGateway();


	// Virtual methods from IDatabaseGateway

	void insertMessage(const Message &message) override;

	void ClearMessage(Message & message) override;


	std::vector<Message> getAllMessagesReceivedByUser(const std::string &username) override;

	virtual void updateGUI() override;

private:

	// Text buffers for ImGUI
	char bufMySqlHost[64] = "citmalumnes.upc.es";
	char bufMySqlPort[64] = "3306";
	char bufMySqlDatabase[64] = "jordior4";
	char bufMySqlTable[64] = "MAIL";
	char bufMySqlUsername[64] = "jordior4";
	char bufMySqlPassword[64] = "41634523sps";
};

