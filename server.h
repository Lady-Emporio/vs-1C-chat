#pragma once
#include <string>


class IAddInDefBase;
void run_daemon_server(IAddInDefBase* base1C);
void server_forever(IAddInDefBase* base1C);
void bind(int *s);
int getMessage(int socket, std::string *message);
void work_with_message(std::string message,std::string * e, IAddInDefBase*base);
void listen(int s);
void sendall(int s,std::string text);


//*********************
void sendAlertAboutNewMessage(IAddInDefBase* base1C, wchar_t* wsData);