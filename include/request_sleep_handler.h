#pragma once

#include <string>
#include "config_parser.h"
#include "request_handler.h"


class request_sleep_handler : public request_handler
{
public:
  request_sleep_handler(std::string location, std::string request_url);
  bhttp::status handle_request(const bhttp::request<bhttp::dynamic_body> req, bhttp::response<bhttp::dynamic_body>& res);

private:
  void log_message_info(std::string res_code);

  std::string location_;
  std::string request_url;
  reply rep;
};
