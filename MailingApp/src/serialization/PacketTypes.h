#pragma once

#include <cstdint>

enum class PacketType : int8_t
{
	LoginRequest,
	QueryAllMessagesRequest,
	QueryAllMessagesResponse,
	SendMessageRequest,
	EraseMessageRequest,
	// Chat ----
	LoginRequestChat,
	ChatMessageRequest,
	// GameCaputured -----
	SendMessageGameCapturedRequest
};
