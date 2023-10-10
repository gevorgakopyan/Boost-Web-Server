#pragma once

#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "session_interface.h"

using boost::asio::ip::tcp;

static const int NUM_THREADS = 4;

class server
{
public:
  server(session_interface& new_s, boost::asio::io_service& io_service,
         short port);
  bool start_accept();
  bool handle_accept(session_interface* new_session,
                     const boost::system::error_code& error);
  bool init_server(NginxConfig config);
  bool set_routes(std::map<std::string, std::shared_ptr<request_handler_factory>> routes);

private:
  boost::asio::io_service& io_service_;
  session_interface& session_;
  tcp::acceptor acceptor_;
  NginxConfig config_;
  std::map<std::string, std::shared_ptr<request_handler_factory>> routes_;
};
