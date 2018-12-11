#pragma once

#include <string>

struct Message
{
	std::string senderUsername;
	std::string receiverUsername;
	std::string subject;
	std::string body;
};

struct MessageGameCaputred
{
	std::string senderUsername;
	std::string Start_Game;
	std::string End_Game;
	std::string Enemies_Killed;
	std::string Gems;
	std::string Hacks;
	std::string keys_pressed;
	std::string dies;
};
