#include "handler_factories/authentication_handler_factory.h"
#include "request_authentication_handler.h"

authentication_handler_factory::authentication_handler_factory(std::string location, NginxConfig config) :
      location_(location), config_(config)
{

}

request_handler* authentication_handler_factory::create(std::string location, std::string request_url, user_profile profile)
{
  std::map<std::string, std::string> path_map = config_.get_auth_path_map(location);
  return new request_authentication_handler(location, request_url, path_map["root"], path_map["data_path"], path_map["signup"], path_map["login"], path_map["logout"], profile);
}