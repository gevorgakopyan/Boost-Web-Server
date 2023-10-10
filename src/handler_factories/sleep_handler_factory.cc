#include "handler_factories/sleep_handler_factory.h"
#include "request_sleep_handler.h"


sleep_handler_factory::sleep_handler_factory(std::string location, NginxConfig config) :
      location_(location), config_(config)
{

}

request_handler* sleep_handler_factory::create(std::string location, std::string request_url, user_profile profile)
{
  return new request_sleep_handler(location, request_url);
}