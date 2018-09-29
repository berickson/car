// socket server
// started from http://www.linuxhowtos.org/C_C++/socket.htm
// http://www.linuxhowtos.org/data/6/server.c

#include "socket_server.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
using namespace std;

void error(const char *msg) {
  perror(msg);
  exit(1);
}

void SocketServer::open_socket(int portno) {
  server_socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  if (server_socket_fd < 0)
    error("ERROR opening socket");
  struct sockaddr_in serv_addr;
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(server_socket_fd, (struct sockaddr *)&serv_addr,
           sizeof(serv_addr)) < 0)
    error("ERROR on binding");
  listen(server_socket_fd, max_connection_count);
}


string SocketServer::get_request() {
  // ensure connected to client
  if (client_socket_fd < 0) {
    client_socket_fd = accept(server_socket_fd, NULL, NULL);
  }
  if (client_socket_fd < 0) {
    return "";
  }


  // read data until newline, then return it
  fd_set readset;
  struct timeval timeout;
  while (true) {

    // read buffer if empty
    if(buffer_next >= buffer_end) {
      buffer_next = 0;
      buffer_end = 0;
      timeout.tv_sec = 0;
      timeout.tv_usec = 10000;
      FD_ZERO(&readset);
      FD_SET(client_socket_fd, &readset);
      int result = select(client_socket_fd + 1, &readset, NULL, NULL, &timeout);
      if(result < 1) return "";

      bzero(buffer, buffer_size);
      buffer_end = recv(client_socket_fd, buffer, buffer_size-1, MSG_DONTWAIT);
      if (buffer_end  == 0) {
        // client disconnected
        close(client_socket_fd);
        client_socket_fd = -1;
        // cout << "client disconnected" << endl;
        return "";
      }
    }
    while(buffer_next < buffer_end) {
      char c = buffer[buffer_next];
      buffer_next++;
      if(c==0){
        string s = ss.str();
        ss.str("");
        ss.clear();
        return s;
      }
      ss << c;
    }
  }


  return "";
}

void SocketServer::send_response(string response) {
  if(client_socket_fd > 0) {
    write(client_socket_fd, response.c_str(), response.length()+1);
  }
}


void play_with_socket_server() {
  SocketServer server;
  server.open_socket(5571);
  while(true) {
    string s = server.get_request();
    if(s.length()) {
      server.send_response(string{"<"}+s+">");
      // cout << s << endl;
    }
  }
}

void test_socket_server() {
    play_with_socket_server();
}

