#include "handler_factories/static_handler_factory.h"
#include "request_static_handler.h"

static_handler_factory::static_handler_factory(std::string location, NginxConfig config) :
  location_(location), config_(config) {}

request_handler* static_handler_factory::create(std::string location, std::string url, user_profile profile)
{
  return new request_static_handler(location, config_.get_root(location), url);
}