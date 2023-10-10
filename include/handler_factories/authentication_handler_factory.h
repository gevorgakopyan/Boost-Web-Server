#pragma once

#include "config_parser.h"
#include "request_handler_factory.h"

/**
 *  Authentication handler factory class.
 *
 *  Inherits from request_handler_factory.
 *
 *  The authentication handler factory constructs a authentication handler factory
 *  by giving the location and path.
 *
 *  The function create will return a request_handler pointer.
 *  In this class, the return pointer points to an authentication handler.
 *
 */
class authentication_handler_factory : public request_handler_factory
{
public:
  authentication_handler_factory(std::string location, NginxConfig config_);

  request_handler* create(std::string location, std::string request_url, user_profile profile);

private:
  std::string location_;
  NginxConfig config_;
};