#include "handler_factories/api_console_handler_factory.h"
#include "api_console_handler.h"

api_console_handler_factory::api_console_handler_factory(std::string location, NginxConfig config) :
      location_(location), config_(config)
{

}

request_handler* api_console_handler_factory::create(std::string location, std::string request_url, user_profile profile)
{
  std::map<std::string, std::string> path_map = config_.get_auth_path_map(location);
  return new api_console_handler(location, request_url, path_map["root"], profile);
}