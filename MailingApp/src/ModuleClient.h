#pragma once

#include "Module.h"
#include "SocketUtils.h"
#include "database/DatabaseTypes.h"
#include "database/DataBaseChat.h"
#include "serialization/MemoryStream.h"

struct ImVec4;

struct MessageResponse
{
	std::string destinatary;
	std::string subject;
	std::string msg_to_respond;
};

class ModuleClient : public Module
{
public:

	// Virtual methods from parent class Module

	bool update() override;

	bool cleanUp() override;

private:

	// Methods involving serialization / deserialization (contain TODOs)

	void updateMessenger();
	void updateChat();

	void onPacketReceived(const InputMemoryStream &stream);

	void onPacketReceivedQueryAllMessagesResponse(const InputMemoryStream &stream);
	void onPacketReceivedChatMessagesResponse(const InputMemoryStream &stream);

	void sendPacketLogin(const char *username);

	void sendPacketLoginChat();

	void sendPacketQueryMessages();

	void sendPacketSendMessage(const char *receiver, const char *subject, const char *message);

	void sendPacketSendMessageChat(const char * message);

	void sendPacketEraseMessage(const char *receiver, const char *subject, const char *message);

	void sendPacket(const OutputMemoryStream &stream);

	
	// GUI

	void updateGUI();

	void updateGUIChat();

	ImVec4 GetColorFromString(std::string color_id);


	// Low-level networking stuff

	void connectToServer();

	void disconnectFromServer();

	void handleIncomingData();

	void handleOutgoingData();


	// Client connection state
	enum class ClientState
	{
		Disconnected,
		Connecting,
		Connected,
		Disconnecting
	};

	// State of the client
	ClientState state = ClientState::Disconnected;

	// IP address of the server
	char serverIP[32] = "127.0.0.1";

	// Port used by the server
	int serverPort = 8000;

	// Socket to connect to the server
	SOCKET connSocket;


	// Current screen of the messenger app
	enum class MessengerState
	{
		SendingLogin,
		RequestingMessages,
		ReceivingMessages,
		ShowingMessages,
		ComposingMessage,
		SendingMessage,
		RespondingMessage,
		EraseMessage
	};

	// Current screen of the messenger app
	enum class ChatState
	{
		SendingLogin,
		ReceivingMessages,
		ShowingMessages,
		SendingMessage,
		EraseMessage
	};

	// Current screen of the messenger application
	MessengerState messengerState = MessengerState::SendingLogin;
	// Current screen of the ChatState application
	ChatState chatState = ChatState::SendingLogin;

	// All messages in the client inbox
	std::vector<Message> messages;
	std::vector<MessageChat> messagesChat;

	//Auxiliar struct to keep sender & subject of a message to RESPOND it
	MessageResponse response;
	bool info_passed = false;
	bool chatWindows = false;

	// Composing Message buffers (for IMGUI)
	char senderBuf[64] = "loginName";   // Buffer for the sender
	char receiverBuf[64]; // Buffer for the receiver
	char subjectBuf[256]; // Buffer for the subject
	char messageBuf[4096];// Buffer for the message

	char receiverBufDel[64]; // Buffer for the receiver
	char subjectBufDel[256]; // Buffer for the subject
	char messageBufDel[4096];// Buffer for the message

	// Chat
	char messageBufChat[4096];// Buffer for the message
	bool scrollChatDown = false;
	bool doscrollChatDown = false;


	// Send and receive buffers (low-level stuff)

	// Recv buffer state
	size_t recvPacketHead = 0;
	size_t recvByteHead = 0;
	std::vector<uint8_t> recvBuffer;

	// Send buffer state
	size_t sendHead = 0;
	std::vector<uint8_t> sendBuffer;
};