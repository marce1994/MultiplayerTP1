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
	int puerto = 8900;

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
	server.sin_port = htons(puerto);

	inet_pton(AF_INET, ip.c_str(), &server.sin_addr);
	SOCKET iosocket = socket(AF_INET, SOCK_DGRAM, 0);
#pragma endregion

	int serverSize = sizeof(server);
	message_t message;

	memset(&message, 0, sizeof(message));
	message.cmd = MSG_CONNECT;

	int sendOk = sendto(iosocket, (char*)&message, sizeof(message), 0, (sockaddr*)&server, sizeof(server));
	if (sendOk == SOCKET_ERROR)
	{
		cout << "Ocurrio un error al conectarse al servidor" << WSAGetLastError() << endl;
		return -1;
	}

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

		if (message.cmd == MSG_PLAY || message.cmd == MSG_UPDATE_BOARD) {
			system("cls");
			aux++;
			cout << "board updated" << aux << endl;
			memcpy(&board, (char*)&message.data, sizeof(board));
			for (size_t i = 0; i < 3; i++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					cout << " " << board[i][j] << " ";
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
			case MSG_SET_ALIAS:
				break;
			case MSG_CONNECT:
				break;
			case MSG_DISCONNECT:
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
				cout << "al lobby pt" << endl;
				return 0;
				break;
			case MSG_UPDATE_BOARD:
				break;
			case MSG_WIN:
				break;
			case MSG_LOOSE:
				break;
		}

		if (sendOk == SOCKET_ERROR)
			cerr << "Hubo un error al enviar" << WSAGetLastError() << message.cmd << " " << message.data << endl;
		else
			cout << "enviado " << message.cmd << " " << message.data << " correctamente" << endl;
	}

	//cerrar el socket
	closesocket(iosocket);

	//cerrar winsock
	WSACleanup();
}
