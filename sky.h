#pragma once
#include <iostream>
#include <Winsock2.h> 
#include <ws2tcpip.h> 
#include <fstream>


#include <wchar.h>
#include <string>
#include <Windows.h>

#include <chrono> 

#include <iomanip>
#include <ctime>
#include <sstream> 

inline int portToS = 13444;
const inline std::string LOG_FILE_PATH = "C:/Users/al/Desktop/1/log.txt";
void log(std::string text);

inline std::string rjust(std::string text, size_t width, char fillchar = '0') {
	if (text.size() >= width) {
		return text;
	}
	std::string returnString= text;
	returnString.insert(0, width - text.size(), fillchar);
	return returnString;
}


inline void fatalError(std::string text) {
	std::stringstream buf;
	buf << "FATAL: " << text;
	log(buf.str());

	exit(99);
}

inline void error(std::string text,int code=-2) {




	std::stringstream buf;
	buf << "ERROR: " << text<< " " << code;
	log(buf.str());
}

inline void log(std::string text) {
	
	//auto time = std::chrono::system_clock::now();
	//auto tm_now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	//tm result;
	//localtime_s(&result, &tm_now);
	//auto now = std::put_time(&result, "%Y.%m.%d %H:%M:%S");

	SYSTEMTIME st;
	GetLocalTime(&st);
	std::stringstream buf;
	buf << st.wYear << "."<< 
	rjust(std::to_string(st.wMonth),2) << "." << 
	rjust(std::to_string(st.wDay), 2) << " " <<
	rjust(std::to_string(st.wHour), 2) <<":"<<
	rjust(std::to_string(st.wMinute), 2) << ":" <<
	rjust(std::to_string(st.wSecond), 2) << "_" <<
	rjust(std::to_string(st.wMilliseconds),3);
	auto now = buf.str();

	std::ofstream logfile;
	logfile.open(LOG_FILE_PATH, std::ios_base::app);
	logfile << now << " | " << text << std::endl;
	logfile.close();
}
inline int createSocket() {
	for (int i=0;i<5;++i){
		SOCKET _s;
		_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_s == INVALID_SOCKET)
		{
			error("createSocket. Error create socket.");
		}
		return _s;
	}
	fatalError("createSocket. Can not create socket.");
	return -1;//linker
}

inline bool isCanRead(int s) {
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(s, &readfds);
	int result = select(
		NULL,
		&readfds,
		NULL,
		NULL,
		&tv
	);
	if (0 == result) {
		return false;
	}
	return true;
}

inline void init()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
		error("WSAStartup failed.");
	}
}

