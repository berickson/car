#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

class SocketServer {
public:
  void open_socket(int portno);
  std::string get_request();
  void send_response(std::string response);


  int server_socket_fd = -1;
  int client_socket_fd = -1;
  struct sockaddr_in serv_addr;
  const int max_connection_count = 1;
  size_t buffer_next = 0;
  size_t buffer_end = 0;
  static const size_t buffer_size = 2000;
  std::stringstream ss;

  char buffer[buffer_size];
};


#endif // SOCKET_SERVER_H
