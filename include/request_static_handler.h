#ifndef REQUEST_STATIC_HANDLER_H
#define REQUEST_STATIC_HANDLER_H

#include "request_handler.h"
#include <iostream>
#include "boost/filesystem.hpp"
#include <filesystem>
#include <string>

class request_static_handler : public request_handler
{
public:
  request_static_handler(std::string location, std::string root, std::string url);
  bhttp::status handle_request(const bhttp::request<bhttp::dynamic_body> req, bhttp::response<bhttp::dynamic_body>& res);

private:
  void log_message_info(std::string res_code);

  std::string method;
  std::string extension;
  std::string root_;
  std::string location_;
  std::string request_url;
  reply rep;
};

#endif