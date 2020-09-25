#pragma once
#include <cstddef>
#include <rpcndr.h>
#include <cstdint>
#include <vcruntime_string.h>

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
	MSG_ASD = 0x00E;
	MSG_ASD = 0x00F;
	MSG_ASD = 0x010;
	MSG_ASD = 0x011;
	MSG_ASD = 0x012;
	MSG_ASD = 0x013;
	MSG_ASD = 0x014;
	MSG_ASD = 0x015;
	MSG_ASD = 0x016;
	MSG_ASD = 0x017;
	MSG_ASD = 0x018;
	*/
};

enum PlayerType {
	EMPTY = -0x001,
	X = 0x00,
	O = 0x001
};

typedef struct {
	uint32_t cmd;
	char data[255];
} message_t;

typedef struct {
	char message[255];
} message_chat_t;