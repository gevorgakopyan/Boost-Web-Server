#include "handler_factories/api_handler_factory.h"
#include "request_api_handler.h"
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>

api_handler_factory::api_handler_factory(std::string location, NginxConfig config) :
  location_(location), config_(config) {
    boost::filesystem::path root_path = config.get_root(location);
    boost::system::error_code ec;
    if(!(boost::filesystem::exists(root_path)))
    {
        boost::filesystem::create_directory(root_path, ec);
        if(ec)
        {
          BOOST_LOG_TRIVIAL(error) << "api_handler_factory::api_handler_factory : could not create root" << ec;
        }
    }
  }

request_handler* api_handler_factory::create(std::string location, std::string url, user_profile profile)
{
  return new request_api_handler(location, config_.get_root(location), url, path_counts, profile);
}