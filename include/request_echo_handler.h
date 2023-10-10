#ifndef REQUEST_ECHO_HANDLER_H
#define REQUEST_ECHO_HANDLER_H

#include "request_handler.h"
#include <string>
#include <vector>

class request_echo_handler: public request_handler
{
public:
  request_echo_handler(std::string location, std::string url);
  bhttp::status handle_request(const bhttp::request<bhttp::dynamic_body> req, bhttp::response<bhttp::dynamic_body>& res);

private:
  void log_message_info(std::string res_code);
  
  std::string location_;
  std::string request_url;
  reply rep;
};

#endif