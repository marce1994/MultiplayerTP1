#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <string.h>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <iomanip>
#include <ctime>
#include <sstream>
#include <vector>
#include <map>
#include <list>
#include "../Sockets.Core/Definitions.h"

using namespace std;

#pragma comment (lib, "ws2_32.lib")
#pragma warning(disable:4996)

const char* getTimestamp() {
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
	std::string str(buffer);

	string msgdata = str;
	return msgdata.c_str();
}

//message_t commandToMessage(string command) {
//	string _command = "";
//	for (size_t i = 0; i < command.length; i++)
//	{
//		if (command[i] == ' ')
//			break;
//		command += command[i];
//	}
//
//	map<string, CommandType> mapeishon;
//	mapeishon["msg"] = MSG_CHAT;
//	mapeishon["alias"] = MSG_SET_ALIAS;
//	mapeishon["disconnect"] = MSG_DISCONNECT;
//
//	message_t message;
//
//	auto pos = mapeishon.find(_command);
//	if (pos == mapeishon.end())
//		cerr << "Comando invalido" << endl;
//	else
//		message.cmd = pos->second;
//
//	auto iterator = mapeishon.find("Jane Smith");
//	mapeishon.at(iterator);
//	std::string s = "scott>=tiger";
//	std::string delimiter = ">=";
//	std::string token = s.substr(0, s.find(delimiter)); // token is "scott"
//}

int main()
{
	string ip = "127.0.0.1";

#pragma region WINSOKS_INIT
	WSADATA data;
	WORD version = MAKEWORD(2, 2);
	//inicializar winsock
	int isOk = WSAStartup(version, &data);
	if (isOk != 0)
	{
		cout << "Cant start!" << isOk << endl;
		return -1;
	}

	sockaddr_in server;

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);

	inet_pton(AF_INET, ip.c_str(), &server.sin_addr);
	SOCKET iosocket = socket(AF_INET, SOCK_DGRAM, 0);
#pragma endregion

	int serverSize = sizeof(server);
	message_t message;

	string alias = "";
	cout << "alias: ";
	cin >> alias;

	memset(&message, 0, sizeof(message));
	message.cmd = MSG_CONNECT;
	memcpy(&message.data, alias.c_str(), sizeof(alias));

	int sendOk = sendto(iosocket, (char*)&message, sizeof(message), 0, (sockaddr*)&server, sizeof(server));
	if (sendOk == SOCKET_ERROR)
	{
		cout << "Ocurrio un error al conectarse al servidor" << WSAGetLastError() << endl;
		return -1;
	}

	std::map<PlayerType, string> first;
	first[O] = "O";
	first[X] = "X";

	int32_t board[3][3] = { { EMPTY, EMPTY, EMPTY}, { EMPTY, EMPTY, EMPTY}, { EMPTY, EMPTY, EMPTY} };
	int32_t aux = 0;

	while (true) {
		memset(&message, 0, sizeof(message));

		auto timestamp = getTimestamp();
		int bytesIn = recvfrom(iosocket, (char*)&message, sizeof(message), 0, (sockaddr*)&server, &serverSize); // Escucha en el puerto.

		if (bytesIn == SOCKET_ERROR) {
			cout << "Error al recibir data" << endl;
			continue; // do nothing
		}

		cout << endl;
		cout << "############# MESSAGE" << message.cmd << "#############" << endl;
		cout << endl;

		if (message.cmd == MSG_PLAY || message.cmd == MSG_UPDATE_BOARD || message.cmd == MSG_WIN || message.cmd == MSG_LOOSE) {
			system("cls");
			memcpy(&board, (char*)&message.data, sizeof(board));
			for (size_t i = 0; i < 3; i++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					int position = (i * 3 + j);
					if (board[i][j] == EMPTY)
						cout << " " << position << " ";
					else
						cout << " " << first[(PlayerType)board[i][j]] << " ";
				}
				cout << endl;
			}
		}

		string command = "";

		switch (message.cmd)
		{
		case MSG_CHAT:
			cout << message.data << endl;
			break;
		case MSG_PLAY:
			memcpy(&board, (char*)&message.data, sizeof(board));

			int32_t imput_number;
			memset(&imput_number, 0, sizeof(imput_number));

			cout << "input: ";
			cin >> imput_number;

			memset(&message, 0, sizeof(message));

			message.cmd = MSG_PLAY;
			memcpy(&message.data, (char*)&imput_number, sizeof(imput_number));

			sendto(iosocket, (char*)&message, sizeof(message), 0, (sockaddr*)&server, sizeof(server));
			break;
		case MSG_KICK:
			cout << "kicked!!! al lobby pt" << endl;
			return 0;
			break;
		case MSG_UPDATE_BOARD:
			break;
		case MSG_WIN: {


			cout << "Ganaste! al lobby pt" << endl;
			string inp;
			cout << "Quiere jugar de nuevo? (s/n)" << endl;
			cin >> inp;
			if (inp == "s") {
				message.cmd = MSG_CONNECT;
				memcpy(&message.data, alias.c_str(), sizeof(alias));
				sendto(iosocket, (char*)&message, sizeof(message), 0, (sockaddr*)&server, sizeof(server));
			}
			else
			{
				return 0;
			}

			cout << "Esperando nueva partida...." << endl;
		}
					break;
		case MSG_LOOSE: {
			cout << "Perdiste, al lobby pt." << endl;
			string inp;
			cout << "Quiere jugar de nuevo? (s/n)" << endl;
			cin >> inp;
			if (inp == "s") {
				message.cmd = MSG_CONNECT;
				memcpy(&message.data, alias.c_str(), sizeof(alias));
				sendto(iosocket, (char*)&message, sizeof(message), 0, (sockaddr*)&server, sizeof(server));
			}
			else
			{
				return 0;
			}

			cout << "Esperando nueva partida...." << endl;
		}
					  break;
		}

		if (sendOk == SOCKET_ERROR)
			cerr << "Hubo un error al enviar" << WSAGetLastError() << message.cmd << " " << message.data << endl;
		/*else
			cout << "enviado " << message.cmd << " " << message.data << " correctamente" << endl;*/
	}

	//cerrar el socket
	closesocket(iosocket);

	//cerrar winsock
	WSACleanup();
}
