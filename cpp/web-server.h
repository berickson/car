#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
//#include <netinet/in.h>
//#include <netinet/tcp.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <map>
#include <regex>
#include <thread>
#include <vector>
#include "string_utils.h"

using namespace std;

class Request {
 public:
  static Request from_string(string s);
  string to_string() const;
  string uri;
  string method;
  string http_version;

  map<string, string> params;

 private:
  Request() {}
  map<string, string> headers;
};

class Response {
 public:
  Response(int fd);
  void add_header(string name, string value);
  void write_status(int code = 200, string description = "OK");
  void enable_multipart();
  void write_content(string mime_type, const char *bytes, size_t byte_count);
  void end();
  bool is_closed();
  int bytes_pending() {
    int value = 0;
    ioctl(fd, SIOCOUTQ, &value);
    return value;
  }
  bool close_requested = false;

 private:
  bool status_written = false;
  bool end_written = false;
  int results_sent = 0;
  const string multipart_boundary = "\r\n--boundarydonotcross\r\n";
  const string multipart_final_boundary = "\r\n--boundarydonotcross--\r\n";
  bool multipart = false;
  map<string, string> headers;

  void write_headers();
  void write(string s);
  void write(const char *bytes, size_t byte_count);


  const int fd;
};

typedef std::function<void(const Request &, Response &)> Handler;

class WebServer {
 public:
  int max_connection_count = 30;
  void add_handler(string method, string uri, Handler handler);
  void run(int port);

 private:
  map<string, Handler> handler_map;
  Handler default_handler = [](const Request &request, Response &response) {
    response.write_status(404, "Not found");
    string message = "Not found";
    response.write_content("text/plain", message.c_str(), message.size());
  };
};
