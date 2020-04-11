
#include "server.h"
#include "nlohmann/json.hpp"
#include <thread>
#include <iostream>
#include <Winsock2.h> 
#include <ws2tcpip.h> 
#include "sky.h"

#include <wchar.h>
#include <string>
#include "AddInNative.h"
using json = nlohmann::json;


void run_daemon_server(IAddInDefBase* base1C)
{
	std::thread thr(server_forever, base1C);
	thr.detach();
}

void server_forever(IAddInDefBase* base1C)
{
	init();
	int server = createSocket();
	bind(&server);
	listen(server);

	struct timeval tv;
	tv.tv_sec = 10;
	tv.tv_usec = 500000;
	std::vector<int>using_sockets;

	while (true) {
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(server, &readfds);
		for (int i : using_sockets) {
			FD_SET(i, &readfds);
		}
		int result = select(
			NULL,
			&readfds,
			NULL,
			NULL,
			&tv
		);
		if (0 == result) {
			log("timeout.");
			continue;
		}
		else if (SOCKET_ERROR == result) {
			error("select error.", WSAGetLastError());
			continue;
		}
		// NEW CONNECTION ######################################
		if (FD_ISSET(server, &readfds)) {
			
			struct sockaddr_storage their_addr;
			socklen_t addr_size;
			addr_size = sizeof their_addr;
			int new_fd = accept(server, (struct sockaddr *)&their_addr, &addr_size);
			log("server_forever(). new connection on:"+ std::to_string(new_fd));
			using_sockets.push_back(new_fd);
		}
		//COME MESSAGE
		std::vector<int> needDelete;
		for (int i : using_sockets) {
			if (FD_ISSET(i, &readfds)) {
				std::string message;
				int res = getMessage(i,&message);
				if (res != 1) {
					needDelete.push_back(i);
				}

				if (res == -1 && message == "") {
					log("server_forever(). Message about close socket on:"+ std::to_string(i));
				}
				else{
					std::string e = "";
					work_with_message(message,&e, base1C);
					if (e != "") {
						log("server_forever(). answer on Error: '"+ message+"'.");
						sendall(i, e);
					}
					else {
						log("server_forever(). answer on Twilight: '" + message + "'.");
						sendall(i, "Twilight");
					}
				}
			}
		}
		for (int i : needDelete) {
			for (auto it = using_sockets.begin(); it != using_sockets.end(); ) {
				if (*it == i) {
					it = using_sockets.erase(it);
				}
				else {
					++it;
				}
			}
		}
		needDelete.clear();
	}
}

void bind(int *s)
{
	for ( int i=0;i!=5;++i){
		
		sockaddr_in service;
		service.sin_family = AF_INET;
		service.sin_port = htons(portToS);

		inet_pton(AF_INET, "127.0.0.1", &(service.sin_addr));

		int iResult = bind(*s, (SOCKADDR *)&service, sizeof(service));
		if (iResult == SOCKET_ERROR) {
			error("bindS. Error with bind server.", WSAGetLastError());
		}
		else {
			log("bindS. Good bind on: '" + std::to_string(portToS) + "'.");
			return;
		}

		*s= createSocket();
		log("bind. bind create socket.");
	}
	fatalError("bind. Can not bind.");
}

int getMessage(int socket,std::string *message)
{
	if (!isCanRead(socket)) {
		error("getMessage. call isCanRead when not messages.", 0);
		return 0;
	}
	*message = "";
	while (isCanRead(socket)) {
		char buf[400 * 8 * 2];
		int byte_count;
		byte_count = recv(socket, buf, sizeof(buf), 0);
		if (0 == byte_count) {
			log("getMessage(). get: recv close connection.");
			closesocket(socket);
			return -1;
		}
		else if (SOCKET_ERROR == byte_count) {
			error("getMessage(). get: recv error.", WSAGetLastError());
			return 0;
		}
		
		buf[byte_count] = '\0';
		*message += buf;
	}
	std::string log_message = "getMessage().  get:";
	log_message += *message;
	log(log_message);
	return 1;

}

void work_with_message(std::string message, std::string * e, IAddInDefBase* base)
{
	std::string name;
	std::string value;
	try
	{
		auto js = json::parse(message);
		name = js.value("name", "-1");
		if ("-1" == name) {
			error("work_with_message. Ошибка в обработке json. Not found name.");
		}
		value = js.value("value", "-1");
		if ("-1" == value) {
			error("work_with_message. Ошибка в обработке json. Not found value.");
		}
		
	}
	catch (...)
	{
		*e = "some error with json parse.";
		error("work_with_message. catch error.");
		return;
	}
	if (name == "newmessage") {
		log("new message: " + value);
		std::wstring wide_string = std::wstring(value.begin(), value.end());
		wchar_t* result = wide_string.data();
		sendAlertAboutNewMessage(base, result);
	}
	else if (name == "ping") {
		log("get ping message");
	}
	else if (name == "comingCall") {
		log("get comingCall message");
		std::wstring wide_string = std::wstring(value.begin(), value.end());
		wchar_t* wsData = wide_string.data();
		
		WCHAR_T* wsSource = L"chatVk";
		WCHAR_T* wsMessage = L"comingCall";
		bool res = base->ExternalEvent(wsSource, wsMessage, wsData);
		if (!res) {
			error("sendAlertAboutNewMessage(). Can not 'ExternalEvent'.");
		}
	}
	else{
		*e = "not know name:"+name;
		error("work_with_message. not know name:" + name);
		return;
	}
}

void listen(int s)
{
	if (listen(s, 10) == SOCKET_ERROR) {
		fatalError("listen error");
	}
}

void sendall(int s, std::string text)
{


		int len;
		char *buf = text.data();
		len = strlen(buf);

		int total = 0;
		int bytesleft = len;
		int n;
		while (total < len) {
			n = send(s, buf + total, bytesleft, 0);

			if (SOCKET_ERROR == n) {
				int result_error = WSAGetLastError();
				switch (result_error) {
				case WSAENOTCONN:
					error("sendAll. Socket is not connected.", result_error);
					break;
				case WSAECONNABORTED:
					error("sendAll. Software caused connection abort.", result_error);
					break;
				case WSAENOTSOCK:
					error("sendAll. Socket operation on nonsocket..", result_error);
					break;
				default:
					error("sendAll. send error", result_error);
				}
				return;
			}

			total += n;
			bytesleft -= n;
		}
}



void sendAlertAboutNewMessage(IAddInDefBase* base1C, WCHAR_T* wsData)
{
	WCHAR_T* wsSource = L"chatVk";
	WCHAR_T* wsMessage = L"newMessage";
	bool res = base1C->ExternalEvent(wsSource, wsMessage, wsData);
	if (!res) {
		error("sendAlertAboutNewMessage(). Can not 'ExternalEvent'.");
	}
}
