#include "handler_factories/echo_handler_factory.h"
#include "request_echo_handler.h"

echo_handler_factory::echo_handler_factory(std::string location, NginxConfig config) :
  location_(location), config_(config) {}

request_handler* echo_handler_factory::create(std::string location, std::string url, user_profile profile)
{
  return new request_echo_handler(location, url);
}