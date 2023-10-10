#pragma once

#include "config_parser.h"
#include "request_handler_factory.h"

class health_handler_factory : public request_handler_factory
{
  public:
    health_handler_factory(std::string location, NginxConfig config);

    request_handler* create(std::string location, std::string request_url, user_profile profile);

  private:
    std::string location_;
    NginxConfig config_;
};
