#ifndef SESSION_INTERFACE_H
#define SESSION_INTERFACE_H

#include <boost/asio.hpp>
#include "config_parser.h"
#include "handler_factories/request_handler_factory.h"

using boost::asio::ip::tcp;


class session_interface
{
public:
  virtual tcp::socket& socket() = 0;
  virtual bool start() = 0;
  virtual session_interface*
    get_session(boost::asio::io_service& io_service) = 0;
  virtual bool set_routes(std::map<std::string, std::shared_ptr<request_handler_factory>> route) = 0;
};

#endif