#pragma once

#include <string>
#include "request_handler.h"

class api_console_handler : public request_handler
{
  public:
    api_console_handler(std::string location, std::string request_url, std::string root, user_profile profile);
    bhttp::status handle_request(const bhttp::request<bhttp::dynamic_body> req, bhttp::response<bhttp::dynamic_body>& res);

  private:
    std::string location_;
    std::string request_url_;
    std::string root_;
    user_profile profile_;
    reply rep_;
};