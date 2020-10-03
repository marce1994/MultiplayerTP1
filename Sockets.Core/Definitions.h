#pragma once
#include <cstddef>
#include <cstdint>

#define PORT 9034

enum CommandType {
	MSG_CHAT = 0x00,
	MSG_SET_ALIAS = 0x001,
	MSG_CONNECT = 0x002,
	MSG_DISCONNECT = 0x003,
	MSG_PLAY = 0x004,
	MSG_KICK = 0x005,
	MSG_UPDATE_BOARD = 0x007,
	MSG_WIN = 0x008,
	MSG_LOOSE = 0x009
	/*
	MSG_ASD = 0x008;
	MSG_ASD = 0x009;
	MSG_ASD = 0x00A;
	MSG_ASD = 0x00B;
	MSG_ASD = 0x00C;
	MSG_ASD = 0x00D;
	*/
};

enum PlayerType {
	EMPTY = -0x001,
	X = 0x001,
	O = 0x002
};

typedef struct {
	uint32_t cmd;
	char data[255];
} message_t;

typedef struct {
	char message[255];
} message_chat_t;