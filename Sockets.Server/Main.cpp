#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <string.h>
#include <vector>
#include <map>
#include <list>
#include "../Sockets.Core/Definitions.h"

#pragma comment (lib, "ws2_32.lib")

using namespace std;

class Game {
public:
	int32_t _board[3][3];

	Game() {
		memset(&_board, 0, sizeof(_board));
		int32_t board[3][3] = { { EMPTY, EMPTY, EMPTY}, { EMPTY, EMPTY, EMPTY}, { EMPTY, EMPTY, EMPTY} };
		memcpy(&_board, &board, sizeof(_board));
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
		else
			return O;
	}

private:

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

	void SentMessage(const message_t& message, const SOCKET& socket_out) {
		int sendOk = sendto(socket_out, (char*)&message, sizeof(message), 0, (sockaddr*)&_client, sizeof(_client));
	}

	void SetAlias(char* alias) {
		memset(_alias, 0, sizeof(_alias));
		memcpy(&_alias, alias, sizeof(_alias));
	}

	char* GetIp() {
		char ip[16];
		inet_ntop(AF_INET, &_client.sin_addr, ip, sizeof(ip));
		return ip;
	}

	bool exists(const vector<Player>& players) {
		for (auto p : players)
			return equals(p);
		return false;
	}

	bool equals(Player& player) {
		if (((string)player.GetIp()) + ((string)player.GetPort()) == ((string)GetIp()) + ((string)GetPort()))
			return true;
	}

	char* GetPort() {
		return (char*)ntohs(_client.sin_port);
	}
};

class GameRoom {
private:
	Player* _player1;
	Player* _player2;
	Game* _game;

public:
	GameRoom(Player* player1, Player* player2) {
		_player1 = player1;
		_player2 = player2;

		_game = new Game();
	}

	void startGame(const SOCKET& socket) {
		message_t message;
		memset(&message, 0, sizeof(message));
		// Envio el update del tablero
		message.cmd = MSG_UPDATE_BOARD;
		memcpy(message.data, _game->_board, sizeof(_game->_board));

		_player1->SentMessage((const message_t&)message, socket);
		_player2->SentMessage((const message_t&)message, socket);

		// Por ahora le doy el turno siempre al player 1
		memset(&message, 0, sizeof(message));
		message.cmd = MSG_PLAY;
		
		_player1->SentMessage((const message_t&)message, socket);
	}

	bool containsPlayer(sockaddr_in& socket)
	{
		auto player = Player(&socket);
		return _player1->equals(player) || _player2->equals(player);
	}

	void play(message_t& message, sockaddr_in& socket)
	{
		auto player = Player(&socket);
		int32_t playedPosition;
		memcpy(&playedPosition, (char*)message.data, sizeof(playedPosition));

		int32_t result = -1;
		if (_player1->equals(player))
			result = _game->Play(O, playedPosition);

		if (_player2->equals(player)) 
			result = _game->Play(X, playedPosition);

		memset(&message, 0, sizeof(message));

		if (result == 1)
		{
			memcpy(message.data, _game->_board, sizeof(_game->_board));

			message.cmd = MSG_WIN;
			_player1->SentMessage((const message_t&)message, (SOCKET&)socket);

			message.cmd = MSG_LOOSE;
			_player2->SentMessage((const message_t&)message, (SOCKET&)socket);
		}

		if (result == 2)
		{
			memcpy(message.data, _game->_board, sizeof(_game->_board));

			message.cmd = MSG_LOOSE;
			_player1->SentMessage((const message_t&)message, (SOCKET&)socket);

			message.cmd = MSG_WIN;
			_player2->SentMessage((const message_t&)message, (SOCKET&)socket);
		}
		
		if (result == 0)
		{
			memset(&message, 0, sizeof(message));

			message.cmd = MSG_UPDATE_BOARD;
			memcpy(message.data, _game->_board, sizeof(_game->_board));
			
			if (_player1->equals(player))
			{
				_player1->SentMessage((const message_t&)message, (SOCKET&)socket);

				message.cmd = MSG_PLAY;
				_player2->SentMessage((const message_t&)message, (SOCKET&)socket);
			}

			if (_player2->equals(player))
			{
				_player2->SentMessage((const message_t&)message, (SOCKET&)socket);

				message.cmd = MSG_PLAY;
				_player1->SentMessage((const message_t&)message, (SOCKET&)socket);
			}
		}
	}

	~GameRoom() {
		delete _game;
		_game = NULL;
	}
};

GameRoom* getRoomPlayer(list<GameRoom>& games, sockaddr_in& player) {
	for (std::list<GameRoom>::iterator it = games.begin(); it != games.end(); ++it)
		if ((*it).containsPlayer(player))
			return &(*it);
	return NULL;
}

int main()
{
#pragma region WINSOCKS_INITIALIZE
	WSADATA data;
	WORD ver = MAKEWORD(2, 2);
	int wsOk = WSAStartup(ver, &data);

	if (wsOk != 0) {
		cerr << "no se pudo iniciar winsock" << endl;
		return -1;
	}

	SOCKET listening = socket(AF_INET, SOCK_DGRAM, 0);
	if (listening == INVALID_SOCKET) {
		cerr << "Invalid socket" << endl;
		return -1;
	}

	// bind el socket (atar el socket a una dupla ip:puerto)
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(8900); // puerto en el que vamos a escuchar
	inet_pton(AF_INET, "127.0.0.1", &hint.sin_addr); // ip loopback

	int bindResult = bind(listening, (sockaddr*)&hint, sizeof(hint)); // bindea el puerto de escucha

	if (bindResult == SOCKET_ERROR) {
		cerr << "No se pudo hacer el bind" << endl;
		return -1;
	}

	SOCKET socket_out = socket(AF_INET, SOCK_DGRAM, 0);
#pragma endregion

	list<GameRoom>* _games = new list<GameRoom>();
	list<Player>* _lobby = new list<Player>();
	
	sockaddr_in client;
	int clientSize = sizeof(client);
	message_t recived_message;
	do {
		char ip[16];
		memset(&ip, 0, sizeof(ip));
		memset(&recived_message, 0, sizeof(recived_message));

		int bytesIn = recvfrom(listening, (char*)&recived_message, sizeof(recived_message), 0, (sockaddr*)&client, &clientSize); // Escucha en el puerto.
		
		if (bytesIn == SOCKET_ERROR) {
			cout << "Error al recibir data" << endl;
			continue; // do nothing
		}

		int sendOk;

		cout << recived_message.cmd << " " << recived_message.data << endl;

		switch (recived_message.cmd)
		{
			case MSG_PLAY:
				{
					cout << "message: " << recived_message.data << endl;

					auto room = getRoomPlayer((list<GameRoom>&)_games, client);
					if (room == NULL)
						continue;
					room->play(recived_message, client);
				}
				break;
			case MSG_CHAT:
				cout << "message: " << recived_message.data << endl;
				sendOk = sendto(socket_out, (char*)&recived_message, bytesIn, 0, (sockaddr*)&client, sizeof(client));
				break;
			case MSG_CONNECT:
				{
					cout << "Message: " << "connect to server" << endl;
					sendOk = sendto(socket_out, (char*)&recived_message, bytesIn, 0, (sockaddr*)&client, sizeof(client));

					auto player = Player(&client);
					if (player.exists((const vector<Player>&)_lobby))
						cout << "player already connected" << endl;
					else
						_lobby->push_back(player);

					if (_lobby->size() >= 2)
					{
						auto p1 = _lobby->front();
						_lobby->pop_front();
						auto p2 = _lobby->front();
						_lobby->pop_front();
						auto room = GameRoom(&p1, &p2);

						cout << "room created" << endl;

						_games->push_back(room);
						room.startGame((const SOCKET&)socket_out);
					}
				}
				break;
			case MSG_DISCONNECT:
				cout << "Message: " << "disconnected" << endl;
				sendOk = sendto(socket_out, (char*)&recived_message, bytesIn, 0, (sockaddr*)&client, sizeof(client));
				break;
			default:
				cerr << "Message: " << "Message type not fount" << endl;
				sendOk = sendto(socket_out, (char*)&recived_message, bytesIn, 0, (sockaddr*)&client, sizeof(client));
				break;
		}

		if (sendOk == SOCKET_ERROR)
			cerr << "Hubo un error al enviar" << WSAGetLastError() << recived_message.cmd << " " << recived_message.data << endl;
		else
			cout << "enviado " << recived_message.cmd << " " << recived_message.data << " correctamente" << endl;
	} while (true);

	// destruir el socket
	closesocket(listening);

	// cleanup winsock
	WSACleanup();
}

// Server
//=======================
// Recibir un continuo de mensajes -> con un while true va
// Recibir IP y puerto del cliente -> crear un sockaddr_in
// Enviar mensajes desde diferentes clientes -> nc.exe
// Responder "mensaje recibido" al cliente. o responder con el mismo mensaje

// Cliente y server
//=======================
// Enviar y recibir una estructura (opcode o algo asi).
// struct:
//		byte cmd;
//		char data[255];
//		
//	cmd == 0
//		=> data es un mensaje de texto
//	cmd == 1
//		=> data, es mi alias en este extranio chat
//
//	Tener una tabla de alias en los clientes, y al imprimir los mensajes, si esta seteado el alias, imprimir
//
//	ALIAS dice: mensaje
//	y sino
//	120.0.0.1:345 dice: mensaje
