#include "handler_factories/handler404factory.h"
#include "request_404_handler.h"


handler404factory::handler404factory(std::string location, NginxConfig config) :
  location_(location), config_(config) {}

request_handler* handler404factory::create(std::string location, std::string url, user_profile profile)
{
  return new request_404_handler(location, url);
}