#include "handler_factories/health_handler_factory.h"
#include "request_health_handler.h"

health_handler_factory::health_handler_factory(std::string location, NginxConfig config) :
      location_(location), config_(config)
{

}

request_handler* health_handler_factory::create(std::string location, std::string request_url, user_profile profile)
{
  return new request_health_handler(location, request_url);
}