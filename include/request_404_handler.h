#ifndef REQUEST_404_HANDLER_H
#define REQUEST_404_HANDLER_H

#include <cstring>
#include "request_handler.h"

class request_404_handler : public request_handler
{
public:
  request_404_handler(std::string location, std::string url);
  bhttp::status handle_request(const bhttp::request<bhttp::dynamic_body> req, bhttp::response<bhttp::dynamic_body>& res);

private:
  void log_message_info(std::string res_code);

  std::string location_;
  std::string request_url;
  reply rep;
};

#endif