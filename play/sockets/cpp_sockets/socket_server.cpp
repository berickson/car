// socket server
// started from http://www.linuxhowtos.org/C_C++/socket.htm
// http://www.linuxhowtos.org/data/6/server.c

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
using namespace std;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

class SocketServer {
public:
  int server_socket_fd = -1;
  int client_socket_fd = -1;
  struct sockaddr_in serv_addr;
  const int max_connection_count = 1;
  size_t buffer_next = 0;
  size_t buffer_end = 0;
  stringstream ss;

  void open_socket(int portno) {
    server_socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (server_socket_fd < 0)
        error("ERROR opening socket");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(server_socket_fd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(server_socket_fd, max_connection_count);
  }

  char buffer[256];

  std::string get_request() {
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
        timeout.tv_usec = 0;
        FD_ZERO(&readset);
        FD_SET(client_socket_fd, &readset);
        int result = select(client_socket_fd + 1, &readset, NULL, NULL, &timeout);
        if(result < 1) return "";

          bzero(buffer, 256);
          buffer_end = recv(client_socket_fd, buffer, 1, MSG_DONTWAIT);
          if (buffer_end  == 0) {
            // client disconnected
            client_socket_fd = -1;
            cout << "client disconnected" << endl;
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

  void send_response(std::string response);
};


void run_socket_server(int portno = 5571)
{
    int server_socket_fd, client_socket_fd;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr;
    int n;
    server_socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (server_socket_fd < 0)
        error("ERROR opening socket");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(server_socket_fd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(server_socket_fd, 5);
    client_socket_fd = accept(server_socket_fd, NULL, NULL);
    if (client_socket_fd < 0)
        error("ERROR on accept");
    fd_set readset;
    struct timeval timeout;
    while (true) {
        cout << "." << std::flush;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        FD_ZERO(&readset);
        FD_SET(client_socket_fd, &readset);
        int result = select(client_socket_fd + 1, &readset, NULL, NULL, &timeout);
        if(result < 1) continue;

        bzero(buffer, 256);
        n = recv(client_socket_fd, buffer, 255, MSG_DONTWAIT);
        if (n == 0) {
            cout << "client disconnected" << endl;
            break;
        }
        // newline is included in the buffer
        cout << "Here is the message: " << buffer << std::flush;
    }
    close(client_socket_fd);
    close(server_socket_fd);
}

void play_with_socket_server() {
  SocketServer server;
  server.open_socket(5571);
  while(true) {
    string s = server.get_request();
    if(s.length()) {
      cout << s << endl;
    }
  }
}

int main(int argc, char **argv)
{
  play_with_socket_server();
    //int portno = (argc == 2) ? atoi(argv[1]) : 5571;
    //run_socket_server(portno);
}
