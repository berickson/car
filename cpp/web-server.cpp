#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string_utils.h"
#include <map>
#include <netinet/in.h>
#include <regex>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
using namespace std;

#include "web_server.h"

Request Request::from_string(string s) {
  Request r;
  auto lines = split(s, "\r\n");
  auto request_line = lines[0];
  auto request_parts = split(request_line, ' ');

  const static std::regex request_regex(R"(^(\w+)\s+(\S+)\s+(.*)$)");
  std::smatch matches;
  std::regex_match(request_line, matches, request_regex);
  if (matches.size() != 4) {
    throw string("bad request line \""+s+"\"");
  }
  r.method = matches[1];
  r.uri = matches[2];
  r.http_version = matches[3];

  const static std::regex header_regex(R"(^(\S*)\:\s*(\S.*)\s*$)");
  for (int i = 1; i < lines.size(); ++i) {
    auto &line = lines[i];
    regex_match(line, matches, header_regex);
    if (matches.size() != 3) {
      break; // end of headers
    }
    r.headers[matches[1]] = matches[2];
  }

  // break apart the command line parameters, if any example /my/path?color=green&fruit=apple
  auto uri_and_params = split(r.uri,'?');
  if(uri_and_params.size()==2) {
    r.uri=uri_and_params[0];
    auto params_string = uri_and_params[1];
    for(auto param_setting : split(params_string,'&')) {
      auto name_value = split(param_setting,'=');
      if(name_value.size()==2) {
        r.params[name_value[0]]=name_value[1];
      }
    }
  }


  return r;
}

string Request::to_string() const {
  stringstream ss;
  ss << "method: " << method << endl;
  ss << "uri: " << uri << endl;
  ss << "http_version: " << http_version << endl;
  for (auto &it : headers) {
    ss << it.first << ": " << it.second << endl;
  }
  ss << endl;
  return ss.str();
}

Response::Response(int fd) : fd(fd) {

}

void Response::add_header(string name, string value) {
  headers[name] = value;
}

void Response::write_status(int code, string description) {
  if(status_written) {
    throw string("Error: Response::write_status called twice in same response");
  }
  string s = (string) "HTTP/1.1 " + to_string(code) + " " + description + "\r\n";
  write(s.c_str(), s.size());
  status_written = true;
}

void Response::enable_multipart() { 
  multipart = true;
  close_requested = true;
}

// writes content-length and bytes, writes good status if no status has been sent
// does nothing if connection is closed
void Response::write_content(string mime_type, const char *bytes, size_t byte_count) {
  if(is_closed()) {
    return;
  }
  if(!status_written) {
    write_status();
  }
  if (multipart) {
    if (results_sent == 0) {
      headers["Content-type"] = "multipart/x-mixed-replace;boundary=--boundarydonotcross";
      headers["Connection"] = "close";
      write_headers();
    }
    write(multipart_boundary);
    headers.clear();
    headers["Content-type"] = mime_type;
    write_headers();
    write(bytes, byte_count);
  } else {
    if(results_sent > 0) {
      throw string("write_content called multiple times in non-multipart response");
    }
    headers["Content-type"] = mime_type;
    headers["Content-length"] = to_string(byte_count);
    write_headers();
    write(bytes, byte_count);
  }
  ++results_sent;
}

// marks end of multi-part response,
// calls after first time ignored

void Response::end() {
  if(end_written) {
    return;
  }
  if (multipart && fd > 0) {
    write(multipart_final_boundary);
  }
  end_written = true;
}

bool Response::is_closed() { 
  char buff[2];
  if (fd <= 0 ||  (recv(fd, buff, 1, MSG_PEEK | MSG_DONTWAIT) == 0)) {
    return true;
  }
  return false;
}

void Response::write_headers() {
  for (auto header : headers) {
    string bytes = header.first + ": " + header.second + "\r\n";
    write(bytes);
  }
  write("\r\n");
}

void Response::write(string s) { write(s.c_str(), s.length()); }

void Response::write(const char *bytes, size_t byte_count) {
  if (fd > 0) {
    int i = 0;
    while (i < byte_count) {
      int written = send(fd, bytes + i, byte_count - i, MSG_NOSIGNAL);
      if (written == -1) {
        throw string("cannot send, client closed");
      }
      i += written;
    }
  }
}

typedef std::function<void(const Request &, Response &)> Handler;

void connection_thread(int client_socket_fd, map<string, Handler> *handler_map, Handler *default_handler) {
  bool trace=false;
  try {
    if(trace) cout << "connected to client socket " << client_socket_fd << endl;

    const size_t buffer_size = 2000;
    char buffer[buffer_size];
    // read request
    fd_set readset;
    struct timeval timeout;

    while (true) {
      timeout.tv_sec = 0;
      timeout.tv_usec = 10000;
      FD_ZERO(&readset);
      FD_SET(client_socket_fd, &readset);
      int result = select(client_socket_fd + 1, &readset, NULL, NULL, &timeout);
      if (result > 0) {
        bzero(buffer, buffer_size);
        auto count_received = recv(client_socket_fd, buffer, buffer_size - 1, MSG_DONTWAIT);
        buffer[count_received] = 0; // terminate string
        if (count_received == 0) {
          if(trace) cout << "client disconnected " << client_socket_fd << endl;
          break;
        }
        Request request = Request::from_string(buffer);
        if(trace) cout << request.method << " " << request.uri << endl;
        Response response(client_socket_fd);

        string handler_key = request.method + request.uri;
        auto it = handler_map->find(handler_key);
        if (it != handler_map->end()) {
          it->second(request, response);
        } else {
          (*default_handler)(request, response);
        }

        if (response.close_requested) {
          if(trace) cout << "response requested close" << endl;
          break;
        }
      }
    }
  } catch (string s) {
    if(trace) cout << "Error in connection thread: " << s << endl;
  }
  if(trace) cout << "closing connection " << client_socket_fd <<  endl;
  close(client_socket_fd);
  if(trace) cout << "leaving connection thread" << endl;
}

void WebServer::add_handler(string method, string uri, Handler handler) { handler_map[method + uri] = handler; }

void WebServer::run(int port) {
  int server_socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  if (server_socket_fd < 0)
    throw(string("ERROR opening socket"));

  linger lin;
  lin.l_onoff = 0;
  lin.l_linger = 0;
  setsockopt(server_socket_fd, SOL_SOCKET, SO_LINGER, (const char *)&lin, sizeof(int));
  int enable = 1;
  if (setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    throw(string("setsockopt(SO_REUSEADDR) failed"));

  struct sockaddr_in serv_addr;
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);
  if (bind(server_socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    throw(string("ERROR on binding"));
  listen(server_socket_fd, max_connection_count);
  cout << "web-server listening on port " << port << endl;

  while (true) {
    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(server_socket_fd, &readfds);
    int select_result = select(server_socket_fd + 1, &readfds, NULL, NULL, &tv);
    if (select_result > 0) {
      auto client_socket_fd = accept(server_socket_fd, NULL, NULL);
      if (client_socket_fd > 0) {
        std::thread t(connection_thread, client_socket_fd, &handler_map, &default_handler);
        pthread_setname_np(t.native_handle(), "web-server-worker");
        t.detach();
      } else {
        static int count = 0;
        ++count;
        cout << "select return " << select_result <<", and no client.  client_socket_fd=" << client_socket_fd << endl;
        cout << "trying to read server_socket_fd to see if things clear up" << endl;
          char buff[80];
          int bytes_read = recv(server_socket_fd, buff, 80, MSG_PEEK | MSG_DONTWAIT);
          cout << "recv returned " << bytes_read << ", this has happened " << count << " times." << endl;
      }
    }
  }
}
