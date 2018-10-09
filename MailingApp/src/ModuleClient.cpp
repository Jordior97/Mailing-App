#include "ModuleClient.h"
#include "Log.h"
#include "imgui/imgui.h"
#include "serialization/PacketTypes.h"
#include <time.h>
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

#define HEADER_SIZE sizeof(uint32_t)
#define RECV_CHUNK_SIZE 4096

bool ModuleClient::update()
{
	updateGUI();
	if (chatWindows)
	{
		updateGUIChat();
	}
	switch (state)
	{
	case ModuleClient::ClientState::Connecting:
		connectToServer();
		break;
	case ModuleClient::ClientState::Connected:
		handleIncomingData();
		updateMessenger();
		if(chatWindows)
			updateChat();
		handleOutgoingData();
		break;
	case ModuleClient::ClientState::Disconnecting:
		disconnectFromServer();
		break;
	default:
		break;
	}

	return true;
}

bool ModuleClient::cleanUp()
{
	disconnectFromServer();
	return true;
}

void ModuleClient::updateMessenger()
{
	switch (messengerState)
	{
	case ModuleClient::MessengerState::SendingLogin:
		sendPacketLogin(senderBuf);
		break;
	case ModuleClient::MessengerState::RequestingMessages:
		sendPacketQueryMessages();
		break;
	case ModuleClient::MessengerState::ReceivingMessages:
		// Idle, do nothing
		break;
	case ModuleClient::MessengerState::ShowingMessages:
		// Idle, do nothing
		break;
	case ModuleClient::MessengerState::ComposingMessage:
		// Idle, do nothing
		break;
	case ModuleClient::MessengerState::SendingMessage:
		sendPacketSendMessage(receiverBuf, subjectBuf, messageBuf);
		break;
	case ModuleClient::MessengerState::EraseMessage:
		sendPacketEraseMessage(receiverBufDel, subjectBufDel, messageBufDel);
		break;
	default:
		break;
	}
}

void ModuleClient::updateChat()
{
	switch (chatState)
	{
	case ModuleClient::ChatState::SendingLogin:
		sendPacketLoginChat();
		break;
	case ModuleClient::ChatState::ReceivingMessages:
		// Idle, do nothing
		break;
	case ModuleClient::ChatState::ShowingMessages:
		// Idle, do nothing
		break;
	case ModuleClient::ChatState::SendingMessage:
		sendPacketSendMessageChat(messageBufChat);
		break;
	case ModuleClient::ChatState::EraseMessage:
		//sendPacketEraseMessage(receiverBufDel, subjectBufDel, messageBufDel);
		break;
	default:
		break;
	}
}

void ModuleClient::onPacketReceived(const InputMemoryStream & stream)
{
	PacketType packetType;
	stream.Read(packetType);

	LOG("onPacketReceived() - packetType: %d", (int)packetType);
	
	switch (packetType)
	{
	case PacketType::QueryAllMessagesResponse:
		onPacketReceivedQueryAllMessagesResponse(stream);
		break;
	case PacketType::ChatMessageRequest:
		onPacketReceivedChatMessagesResponse(stream);
		break;
	default:
		LOG("Unknown packet type received");
		break;
	}
}

void ModuleClient::onPacketReceivedQueryAllMessagesResponse(const InputMemoryStream & stream)
{
	messages.clear();

	uint32_t messageCount;

	// TODO: Deserialize the number of messages
	stream.Read(messageCount);

	// TODO: Deserialize messages one by one and push_back them into the messages vector
	// NOTE: The messages vector is an attribute of this class
	for (int i = 0; i < messageCount; i++)
	{
		Message message;

		stream.Read(message.senderUsername);
		stream.Read(message.receiverUsername);
		stream.Read(message.subject);
		stream.Read(message.body);

		messages.push_back(message);
	}

	messengerState = MessengerState::ShowingMessages;
}

void ModuleClient::onPacketReceivedChatMessagesResponse(const InputMemoryStream & stream)
{
	messagesChat.clear();

	uint32_t messageCount;

	// TODO: Deserialize the number of messages
	stream.Read(messageCount);

	// TODO: Deserialize messages one by one and push_back them into the messages vector
	// NOTE: The messages vector is an attribute of this class
	for (int i = 0; i < messageCount; i++)
	{
		MessageChat message;

		stream.Read(message.senderUsername);
		stream.Read(message.date);
		stream.Read(message.body);
		stream.Read(message.color);

		messagesChat.push_back(message);
	}

	chatState = ChatState::ReceivingMessages;
}

void ModuleClient::sendPacketLogin(const char * username)
{
	OutputMemoryStream stream;
	std::string username_str;
	username_str = username;

	// TODO: Serialize Login (packet type and username)
	stream.Write(PacketType::LoginRequest);
	stream.Write(username_str);

	// TODO: Use sendPacket() to send the packet
	sendPacket(stream);

	messengerState = MessengerState::RequestingMessages;
}

void ModuleClient::sendPacketLoginChat()
{
	OutputMemoryStream stream;

	// TODO: Serialize Login (packet type and username)
	stream.Write(PacketType::LoginRequestChat);

	// TODO: Use sendPacket() to send the packet
	sendPacket(stream);

	chatState = ChatState::ReceivingMessages;
}

void ModuleClient::sendPacketQueryMessages()
{
	OutputMemoryStream stream;

	// TODO: Serialize message (only the packet type)
	stream.Write(PacketType::QueryAllMessagesRequest);

	// TODO: Use sendPacket() to send the packet
	sendPacket(stream);

	messengerState = MessengerState::ReceivingMessages;
}

void ModuleClient::sendPacketSendMessage(const char * receiver, const char * subject, const char *message)
{
	OutputMemoryStream stream;

	// TODO: Serialize message (packet type and all fields in the message)
	// NOTE: remember that senderBuf contains the current client (i.e. the sender of the message)
	std::string sender_str(senderBuf);
	std::string receiver_str(receiver);
	std::string subject_str(subject);
	std::string message_str(message);

	stream.Write(PacketType::SendMessageRequest);
	stream.Write(sender_str);
	stream.Write(receiver_str);
	stream.Write(subject_str);
	stream.Write(message_str);

	// TODO: Use sendPacket() to send the packet
	sendPacket(stream);

	messengerState = MessengerState::RequestingMessages;
}

void ModuleClient::sendPacketSendMessageChat(const char *message)
{
	OutputMemoryStream stream;

	// TODO: Serialize message (packet type and all fields in the message)
	// NOTE: remember that senderBuf contains the current client (i.e. the sender of the message)
	std::string sender_str(senderBuf);

	// Get World Time
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%X", &tstruct);
	std::string date_str(buf);

	std::string message_str(message);

	stream.Write(PacketType::ChatMessageRequest);
	stream.Write(sender_str);
	stream.Write(date_str);
	stream.Write(message_str);

	// TODO: Use sendPacket() to send the packet
	sendPacket(stream);

	strcpy_s(messageBufChat, "");

	chatState = ChatState::ReceivingMessages;
}

void ModuleClient::sendPacketEraseMessage(const char * receiver, const char * subject, const char * message)
{
	OutputMemoryStream stream;

	// TODO: Serialize message (packet type and all fields in the message)
	// NOTE: remember that senderBuf contains the current client (i.e. the sender of the message)
	std::string sender_str(senderBuf);
	std::string receiver_str(receiver);
	std::string subject_str(subject);
	std::string message_str(message);

	stream.Write(PacketType::EraseMessageRequest);
	stream.Write(sender_str);
	stream.Write(receiver_str);
	stream.Write(subject_str);
	stream.Write(message_str);

	// TODO: Use sendPacket() to send the packet
	sendPacket(stream);

	messengerState = MessengerState::RequestingMessages;
}

// This function is done for you: Takes the stream and schedules its internal buffer to be sent
void ModuleClient::sendPacket(const OutputMemoryStream & stream)
{
	// Copy the packet into the send buffer
	size_t oldSize = sendBuffer.size();
	sendBuffer.resize(oldSize + HEADER_SIZE + stream.GetSize());
	uint32_t &packetSize = *(uint32_t*)&sendBuffer[oldSize];
	packetSize = HEADER_SIZE + stream.GetSize(); // header size + payload size
	//std::copy(stream.GetBufferPtr(), stream.GetBufferPtr() + stream.GetSize(), &sendBuffer[oldSize] + HEADER_SIZE);
	memcpy(&sendBuffer[oldSize] + HEADER_SIZE, stream.GetBufferPtr(), stream.GetSize());
}


// GUI: Modify this to add extra features...

void ModuleClient::updateGUI()
{
	ImGui::Begin("Client Window");


	if (state == ClientState::Disconnected)
	{
		if (ImGui::CollapsingHeader("Server data", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// IP address
			static char ipBuffer[64] = "127.0.0.1";
			ImGui::InputText("IP", ipBuffer, sizeof(ipBuffer));

			// Port
			static int port = 8000;
			ImGui::InputInt("Port", &port);

			// Connect button
			ImGui::InputText("Login name", senderBuf, sizeof(senderBuf));

			if (ImGui::Button("Connect"))
			{
				if (state == ClientState::Disconnected)
				{
					state = ClientState::Connecting;
				}
			}
		}
	}
	else if (state == ClientState::Connected)
	{
		// Disconnect button
		if (ImGui::Button("Disconnect"))
		{
			if (state == ClientState::Connected)
			{
				state = ClientState::Disconnecting;
			}
		}

		// Chat button
		if (chatWindows == false)
		{
			if (ImGui::Button("Connect Chat"))
			{
				chatWindows = true;
			}
		}
		else
		{
			if (ImGui::Button("Disconnect Chat"))
			{

			}
		}

		if (messengerState == MessengerState::ComposingMessage)
		{
			ImGui::InputText("Receiver", receiverBuf, sizeof(receiverBuf));
			ImGui::InputText("Subject", subjectBuf, sizeof(subjectBuf));
			ImGui::InputTextMultiline("Message", messageBuf, sizeof(messageBuf));
			if (ImGui::Button("Send"))
			{
				messengerState = MessengerState::SendingMessage;
			}
			if (ImGui::Button("Discard"))
			{
				messengerState = MessengerState::ShowingMessages;
			}
		}
		else if (messengerState == MessengerState::RespondingMessage)
		{
			//Pass the data into the buffers (only once)
			if (!info_passed)
			{
				strcpy_s(receiverBuf, response.destinatary.c_str());
				strcpy_s(subjectBuf, response.subject.c_str());
				strcpy_s(messageBuf, "");
				info_passed = true;
			}

			ImGui::Text("Receiver: %s", response.destinatary.c_str());
			ImGui::Text("Subject: %s", response.subject.c_str());
			ImGui::TextWrapped("Message: %s", response.msg_to_respond.c_str());
			
			ImGui::InputTextMultiline("Response", messageBuf, sizeof(messageBuf));
			if (ImGui::Button("Send"))
			{
				messengerState = MessengerState::SendingMessage;
			}
			if (ImGui::Button("Discard"))
			{
				messengerState = MessengerState::ShowingMessages;
			}
		}
		else if (messengerState == MessengerState::ShowingMessages)
		{
			if (ImGui::Button("Compose message"))
			{
				//Clean buffers
				strcpy_s(receiverBuf, "");
				strcpy_s(subjectBuf, "");
				strcpy_s(messageBuf, "");

				messengerState = MessengerState::ComposingMessage;
			}

			if (ImGui::Button("Refresh inbox"))
			{
				messengerState = MessengerState::RequestingMessages;
			}

			ImGui::Text("Inbox:");

			if (messages.empty()) {
				ImGui::Text(" - Your inbox is empty.");
			}

			int i = 0;

			for (auto &message : messages)
			{
				ImGui::PushID(i++);

				if (ImGui::TreeNode(&message, "%s - %s", message.senderUsername.c_str(), message.subject.c_str()))
				{
					//MESSAGE TEXT
					ImGui::TextWrapped("%s", message.body.c_str());

					//RESPONSE BUTTON: Send back a response to the sender of the message
					if (ImGui::Button("Respond"))
					{
						//Keep the info of sender, subject & message
						response.destinatary = message.senderUsername;
						response.subject = "RE: " + message.subject;
						response.msg_to_respond = message.body;
						info_passed = false;

						messengerState = MessengerState::RespondingMessage;
					}

					ImGui::SameLine();

					//DELETE BUTTON: Delete the selected message
					if (ImGui::Button("Delete"))
					{
						strcpy_s(receiverBufDel, message.receiverUsername.c_str());
						strcpy_s(subjectBufDel, message.subject.c_str());
						strcpy_s(messageBufDel, message.body.c_str());

						messengerState = MessengerState::EraseMessage;
					}
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
		}
	}

	ImGui::End();

}

void ModuleClient::updateGUIChat()
{
	ImGui::Begin("Chat Windows");

	for (int i = 0; i < messagesChat.size(); i++)
	{
		ImGui::TextColored(GetColorFromString(messagesChat[i].color), messagesChat[i].senderUsername.c_str()); ImGui::SameLine();
		ImGui::Text("             "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.85, 0.85, 0.85, 1), messagesChat[i].date.c_str());
		ImGui::Text("    "); ImGui::SameLine();
		ImGui::TextWrapped(messagesChat[i].body.c_str());
	}

	ImGui::SetCursorPos(ImVec2(0, ImGui::GetWindowHeight() - 30));
	//ImGui::Button("->", ImVec2(0, ImGui::GetWindowHeight() - 20)); ImGui::SameLine();
	ImGui::Separator();
	if (ImGui::InputText("Message", messageBufChat, sizeof(messageBufChat), ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
	{
		chatState = ChatState::SendingMessage;
	}
	ImGui::SameLine();
	if (ImGui::Button("Send"))
	{
		chatState = ChatState::SendingMessage;
	}
	ImGui::End();

}

ImVec4 ModuleClient::GetColorFromString(std::string color_id)
{
	if (color_id.compare("0") == 0)
	{
		return ImVec4(0, 1, 1, 1);
	}
	if (color_id.compare("1") == 0)
	{
		return ImVec4(0, 1, 0, 1);
	}
	if (color_id.compare("2") == 0)
	{
		return ImVec4(0, 0, 1, 1);
	}
	if (color_id.compare("3") == 0)
	{
		return ImVec4(1, 1, 0, 1);
	}
	if (color_id.compare("4") == 0)
	{
		return ImVec4(1, 0, 0, 1);
	}
	if (color_id.compare("5") == 0)
	{
		return ImVec4(1, 0, 1, 1);
	}
}

// Low-level networking stuff...

void ModuleClient::connectToServer()
{
	// Create socket
	connSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (connSocket == INVALID_SOCKET)
	{
		printWSErrorAndExit("socket()");
	}

	// Connect
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverPort);
	inet_pton(AF_INET, serverIP, &serverAddr.sin_addr);
	int res = connect(connSocket, (const sockaddr*)&serverAddr, sizeof(serverAddr));
	if (res == SOCKET_ERROR)
	{
		printWSError("connect()");
		LOG("Could not connect to the server %s:%d", serverIP, serverPort);
		state = ClientState::Disconnecting;
	}
	else
	{
		state = ClientState::Connected;
		LOG("Server connected to %s:%d", serverIP, serverPort);

		messengerState = MessengerState::SendingLogin;
	}

	// Set non-blocking socket
	u_long nonBlocking = 1;
	res = ioctlsocket(connSocket, FIONBIO, &nonBlocking);
	if (res == SOCKET_ERROR) {
		printWSError("ioctlsocket() non-blocking");
		LOG("Could not set the socket in non-blocking mode.", serverIP, serverPort);
		state = ClientState::Disconnecting;
	}
}

void ModuleClient::disconnectFromServer()
{
	closesocket(connSocket);
	recvBuffer.clear();
	recvPacketHead = 0;
	recvByteHead = 0;
	sendBuffer.clear();
	sendHead = 0;
	state = ClientState::Disconnected;
}

void ModuleClient::handleIncomingData()
{
	if (recvBuffer.size() - recvByteHead < RECV_CHUNK_SIZE) {
		recvBuffer.resize(recvByteHead + RECV_CHUNK_SIZE);
	}

	int res = recv(connSocket, (char*)&recvBuffer[recvByteHead], RECV_CHUNK_SIZE, 0);
	if (res == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			// Do nothing
		}
		else
		{
			printWSError("recv() - socket disconnected forcily");
			state = ClientState::Disconnecting;
		}
	}
	else
	{
		if (res == 0)
		{
			state = ClientState::Disconnecting;
			LOG("Disconnection from server");
			return;
		}

		recvByteHead += res;
		while (recvByteHead - recvPacketHead > HEADER_SIZE)
		{
			const size_t recvWindow = recvByteHead - recvPacketHead;
			const uint32_t packetSize = *(uint32_t*)&recvBuffer[recvPacketHead];
			if (recvWindow >= packetSize)
			{
				InputMemoryStream stream(packetSize - HEADER_SIZE);
				//std::copy(&recvBuffer[recvPacketHead + HEADER_SIZE], &recvBuffer[recvPacketHead + packetSize], (uint8_t*)stream.GetBufferPtr());
				memcpy(stream.GetBufferPtr(), &recvBuffer[recvPacketHead + HEADER_SIZE], packetSize - HEADER_SIZE);
				onPacketReceived(stream);
				recvPacketHead += packetSize;
			}
		}

		if (recvPacketHead >= recvByteHead)
		{
			recvPacketHead = 0;
			recvByteHead = 0;
		}
	}
}

void ModuleClient::handleOutgoingData()
{
	if (sendHead < sendBuffer.size())
	{
		int res = send(connSocket, (const char *)&sendBuffer[sendHead], (int)sendBuffer.size(), 0);
		if (res == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				// Do nothing
			}
			else
			{
				printWSError("send()");
				state = ClientState::Disconnecting;
			}
		}
		else
		{
			sendHead += res;
		}

		if (sendHead >= sendBuffer.size())
		{
			sendHead = 0;
			sendBuffer.clear();
		}
	}
}