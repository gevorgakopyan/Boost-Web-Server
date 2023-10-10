#pragma once

#include <string>
#include "request_handler.h"
#include "config_parser.h"
#include "user_profile.h"

class request_handler_factory
{
public:
  virtual request_handler* create(std::string location, std::string url, user_profile profile) = 0;
  virtual ~request_handler_factory() {}
};

