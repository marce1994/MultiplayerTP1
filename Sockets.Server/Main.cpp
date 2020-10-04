
#ifdef _WIN32
// taken from https://stackoverflow.com/questions/28027937/cross-platform-sockets
/* See http://stackoverflow.com/questions/12765743/getaddrinfo-on-win32 */
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501  /* Windows XP. */
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#else
/* Assume that any non-Windows platform uses POSIX-style sockets instead. */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
#include <unistd.h> /* Needed for close() */
typedef int SOCKET;
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#define INT_MAX       2147483647
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "../Sockets.Core/Definitions.h"

#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <thread>

using namespace std;

#pragma region SOCKS_FUNCTIONS
int sockInit(void)
{
#ifdef _WIN32
	WSADATA wsa_data;
	return WSAStartup(MAKEWORD(1, 1), &wsa_data);
#else
	return 0;
#endif
}

int sockQuit(void)
{
#ifdef _WIN32
	return WSACleanup();
#else
	return 0;
#endif
}

/* Note: For POSIX, typedef SOCKET as an int. */
int sockClose(SOCKET sock)
{
	int status = 0;
#ifdef _WIN32
	status = shutdown(sock, SD_BOTH);
	if (status == 0) { status = closesocket(sock); }
#else
	status = shutdown(sock, SHUT_RDWR);
	if (status == 0) { status = close(sock); }
#endif
	return status;
}
#pragma endregion

class Game {
public:
	int32_t _board[3][3];

	Game()
	{
		memset(&_board, EMPTY, sizeof(_board));
	}

	int32_t Play(PlayerType player, int32_t position) {
		switch (position)
		{
		case 0:
			if (_board[0][0] != EMPTY)
				return -1;
			_board[0][0] = player;
			break;
		case 1:
			if (_board[0][1] != EMPTY)
				return -1;
			_board[0][1] = player;
			break;
		case 2:
			if (_board[0][2] != EMPTY)
				return -1;
			_board[0][2] = player;
			break;
		case 3:
			if (_board[1][0] != EMPTY)
				return -1;
			_board[1][0] = player;
			break;
		case 4:
			if (_board[1][1] != EMPTY)
				return -1;
			_board[1][1] = player;
			break;
		case 5:
			if (_board[1][2] != EMPTY)
				return -1;
			_board[1][2] = player;
			break;
		case 6:
			if (_board[2][0] != EMPTY)
				return -1;
			_board[2][0] = player;
			break;
		case 7:
			if (_board[2][1] != EMPTY)
				return -1;
			_board[2][1] = player;
			break;
		case 8:
			if (_board[2][2] != EMPTY)
				return -1;
			_board[2][2] = player;
			break;
		default:
			return -1;
		}

		auto result = Heuristic();

		if (result > 0)
			return X;
		else if (result < 0)
			return O;
		else
			return 0;
	}

	int32_t Heuristic() {
		for (int i = 0; i < 3; i++)
		{
			if (_board[i][0] == _board[i][1] && _board[i][1] == _board[i][2])
			{
				if (_board[i][0] == O)
					return 10;
				else if (_board[i][0] == X)
					return -10;
			}
		}

		for (int j = 0; j < 3; j++)
		{
			if (_board[0][j] == _board[1][j] &&
				_board[1][j] == _board[2][j])
			{
				if (_board[0][j] == O)
					return 10;
				else if (_board[0][j] == X)
					return -10;
			}
		}

		if (_board[0][0] == _board[1][1] && _board[1][1] == _board[2][2])
		{
			if (_board[0][0] == O)
			{
				return 10;
			}
			else if (_board[0][0] == X)
				return -10;
		}

		if (_board[0][2] == _board[1][1] && _board[1][1] == _board[2][0])
		{
			if (_board[0][2] == O)
			{
				return 10;
			}
			else if (_board[0][2] == X)
				return -10;
		}

		return 0;
	}

	bool isMovesLeft()
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				if (_board[i][j] == EMPTY)
					return true;
		return false;
	}

	//int32_t MiniMax(int32_t depth, bool maximizingPlayer)
	//{
	//	auto euristic = Heuristic();
	//	if (depth == 0)
	//		return euristic;
	//	if (isMovesLeft() == false)
	//		return 0;

	//	if (maximizingPlayer)
	//	{
	//		int32_t value = INT32_MIN;
	//		for (int i = 0; i < 3; i++)
	//			for (int j = 0; j < 3; j++)
	//				if (_board[i][j] == EMPTY)
	//				{
	//					_board[i][j] = O;
	//					value = max(value, MiniMax(depth - 1, false));
	//					_board[i][j] = EMPTY;
	//				}
	//		return value;
	//	}

	//	else
	//	{
	//		int32_t best = INT32_MAX;
	//		for (int i = 0; i < 3; i++)
	//			for (int j = 0; j < 3; j++)
	//				if (_board[i][j] == EMPTY)
	//				{
	//					_board[i][j] = X;
	//					best = min(best, MiniMax(_board, depth - 1, true));
	//					_board[i][j] = EMPTY;
	//				}
	//		return best;
	//	}
	//}
};

class Player {
private:
	sockaddr_in _client;
	char _alias[255];

public:

	Player(sockaddr_in* client) {
		memset(&_client, 0, sizeof(_client));
		memcpy(&_client, client, sizeof(_client));
	}

	void SentMessage(message_t* message, SOCKET* socket_out) {
		int sendOk = sendto(*socket_out, (char*)message, sizeof(*message), 0, (sockaddr*)&_client, sizeof(_client));
	}

	void SetAlias(char* alias) {
		memset(_alias, 0, sizeof(_alias));
		memcpy(&_alias, alias, sizeof(_alias));
		cout << "Player: " << _alias << endl;
	}

	bool exists(const vector<Player>& players) {
		for (Player p : players)
			return equals(p);
		return false;
	}

	bool equals(Player& player) {
		if (memcmp(&player._client.sin_port, &_client.sin_port, sizeof(_client.sin_port)) == 0)
		{
			if (memcmp(&player._client.sin_addr, &_client.sin_addr, sizeof(_client.sin_addr)) == 0)
			{
				return true;
			}
		}
		return false;
	}
};

class GameRoom {
private:
	Player* _player1;
	Player* _player2;
	Game* _game;
	

public:
	int guidGame;

	GameRoom(Player* player1, Player* player2) {
		_player1 = player1;
		_player2 = player2;

		_game = new Game();
		
		guidGame = rand() % INT_MAX;
	}

	bool gameEnded()
	{
		return _game->Heuristic() != 0;
	}

	void startGame(SOCKET* socket) {
		message_t message;
		memset(&message, 0, sizeof(message));
		memcpy(message.data, _game->_board, sizeof(_game->_board));
		
		int random = rand() % 100 + 1;

		if (random > 50) {
			message.cmd = MSG_PLAY;
			_player1->SentMessage(&message, socket);
			message.cmd = MSG_UPDATE_BOARD;
			_player2->SentMessage(&message, socket);
		}
		else {
			message.cmd = MSG_UPDATE_BOARD;
			_player1->SentMessage(&message, socket);
			message.cmd = MSG_PLAY;
			_player2->SentMessage(&message, socket);
		}
	}

	bool containsPlayer(sockaddr_in* socket)
	{
		auto player = Player(socket);
		return _player1->equals(player) || _player2->equals(player);
	}

	void play(message_t* message, sockaddr_in* player_socket, SOCKET* out_socket)
	{
		Player player = Player(player_socket);
		int32_t playedPosition;
		memcpy(&playedPosition, &message->data, sizeof(playedPosition));

		int32_t result = -1;
		if (_player1->equals(player)) {
			result = _game->Play(O, playedPosition);
			memset(message, 0, sizeof(*message));
			if (result < 0)
			{
				memset(message, 0, sizeof(*message));
				message->cmd = MSG_PLAY;
				memcpy(&message->data, (char*)&_game->_board, sizeof(_game->_board));
				_player1->SentMessage(message, out_socket);
			}
			if (result == 0)
			{
				memset(message, 0, sizeof(*message));
				message->cmd = MSG_UPDATE_BOARD;
				memcpy(&message->data, (char*)&_game->_board, sizeof(_game->_board));
				_player1->SentMessage(message, out_socket);

				memset(message, 0, sizeof(*message));
				message->cmd = MSG_PLAY;
				memcpy(&message->data, (char*)&_game->_board, sizeof(_game->_board));
				_player2->SentMessage(message, out_socket);
			}
			if (result > 0)
			{
				memcpy(&message->data, (char*)&_game->_board, sizeof(_game->_board));

				message->cmd = MSG_LOOSE;
				_player1->SentMessage(message, out_socket);

				message->cmd = MSG_WIN;
				_player2->SentMessage(message, out_socket);
			}
		}
		else if (_player2->equals(player)){
			result = _game->Play(X, playedPosition);
			memset(message, 0, sizeof(*message));
			if (result < 0)
			{
				memset(message, 0, sizeof(*message));
				message->cmd = MSG_PLAY;
				memcpy(&message->data, (char*)&_game->_board, sizeof(_game->_board));
				_player2->SentMessage(message, out_socket);
			}
			if (result == 0)
			{
				memset(message, 0, sizeof(*message));
				message->cmd = MSG_UPDATE_BOARD;
				memcpy(&message->data, (char*)&_game->_board, sizeof(_game->_board));
				_player2->SentMessage(message, out_socket);

				memset(message, 0, sizeof(*message));
				message->cmd = MSG_PLAY;
				memcpy(&message->data, (char*)&_game->_board, sizeof(_game->_board));
				_player1->SentMessage(message, out_socket);
			}
			if (result > 0)
			{
				memcpy(&message->data, (char*)&_game->_board, sizeof(_game->_board));

				message->cmd = MSG_LOOSE;
				_player1->SentMessage(message, out_socket);

				message->cmd = MSG_WIN;
				_player2->SentMessage(message, out_socket);
			}
		}
	}

	bool operator == (const GameRoom& s) const { return guidGame == s.guidGame; }
	bool operator != (const GameRoom& s) const { return !operator==(s); }
};

GameRoom* getRoomPlayer(list<GameRoom>* games, sockaddr_in* player) {
	for (std::list<GameRoom>::iterator it = games->begin(); it != games->end(); ++it)
	{
		if ((*it).containsPlayer(player))
		{
			return &(*it);
		}
	}
	return NULL;
}

int main()
{
#pragma region WINSOCKS_INITIALIZE
	fd_set master;	// Master file descriptor list
	SOCKET listener = socket(AF_INET, SOCK_DGRAM, 0);

	// bind el socket (atar el socket a una dupla ip:puerto)
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	struct sockaddr_in sender;

	sockInit();
	listener = socket(AF_INET, SOCK_DGRAM, 0);
	if (listener == INVALID_SOCKET || listener < 0) {
		exit(1);
	}

	//fixme: check error values for unix
	if (bind(listener, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		sockClose(listener);
		exit(2);
	}
#pragma endregion

	list<GameRoom>* _games = new list<GameRoom>();
	list<Player>* _lobby = new list<Player>();

	message_t recived_message;

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 10000; // 10ms
	
	int nbytes;

	int n;
	socklen_t senderSize = sizeof(sender);

	for (;;) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		memset(&recived_message, 0, sizeof(recived_message));

		FD_ZERO(&master);
		FD_SET(listener, &master);

		n = select(listener, &master, NULL, NULL, &tv);

		if (n > 0) {
			// leer el socket
			nbytes = recvfrom(listener, (char*)&recived_message, sizeof(recived_message), 0, (sockaddr*)&sender, &senderSize);

			if (nbytes > 0) {
				cout << "rooms:" << endl;
				for (GameRoom room : *_games)
				{
					cout << room.guidGame << "-";
				}
				cout << endl;

				int sendOk = 0;

				switch (recived_message.cmd)
				{
				case MSG_PLAY:
				{
					int32_t test;
					memset(&test, 0, sizeof(test));
					memcpy(&test, (char*)&recived_message.data, sizeof(test));

					GameRoom* room = getRoomPlayer(_games, &sender);
					if (room == NULL)
						continue;
					room->play(&recived_message, &sender, &listener);

					if (room->gameEnded())
						_games->remove(*room);
				}
				break;
				case MSG_CHAT:
					cout << "message: " << recived_message.data << endl;
					sendOk = sendto(listener, (char*)&recived_message, nbytes, 0, (sockaddr*)&sender, sizeof(sender));
					break;
				case MSG_CONNECT:
				{
					cout << "Message: " << "connect to server" << endl;
					sendOk = sendto(listener, (char*)&recived_message, nbytes, 0, (sockaddr*)&sender, sizeof(sender));

					Player player = Player(&sender);
					player.SetAlias((char*)&recived_message.data);

					if (player.exists((const vector<Player>&)_lobby))
						cout << "player already connected" << endl;
					else
						_lobby->push_back(player);

					if (_lobby->size() >= 2)
					{
						Player p1 = _lobby->front();
						_lobby->pop_front();
						Player p2 = _lobby->front();
						_lobby->pop_front();
						GameRoom room = GameRoom(&p1, &p2);

						cout << "room created" << endl;

						_games->push_back(room);
						room.startGame(&listener);
					}
				}
				break;
				case MSG_DISCONNECT:
					cout << "Message: " << "disconnected" << endl;
					sendOk = sendto(listener, (char*)&recived_message, nbytes, 0, (sockaddr*)&sender, sizeof(sender));
					break;
				default:
					cerr << "Message: " << "Message type not fount" << endl;
					sendOk = sendto(listener, (char*)&recived_message, nbytes, 0, (sockaddr*)&sender, sizeof(sender));
					break;
				}

				if (sendOk == SOCKET_ERROR)
					cerr << "Hubo un error al enviar" << recived_message.cmd << " " << recived_message.data << endl;
			}
		}
		else if (n < 0) {
			perror("select");
			exit(4);
		}
	}

	sockQuit();

	return 0;
}
