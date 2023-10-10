#pragma once

#include "request_handler_factory.h"
 
class api_console_handler_factory : public request_handler_factory
{
  public:
    api_console_handler_factory(std::string location, NginxConfig config);
    request_handler* create(std::string location, std::string request_url, user_profile profile);

  private:
    std::string location_;
    NginxConfig config_;
};
