#pragma once

#include "session_interface.h"
#include "request_handler.h"
#include "request_parser.h"
#include "request.h"
#include "user_profile.h"

#include <boost/asio/ip/address.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <iostream>

using boost::asio::ip::tcp;

class session : public session_interface
{
public:
  session(boost::asio::io_service& io_service);
  tcp::socket& socket();
  boost::asio::streambuf buf;
  bool start();
  std::string handle_read(const boost::system::error_code& error,
                          size_t bytes_transferred);
  bool handle_write(const boost::system::error_code& error);
  session_interface* get_session(boost::asio::io_service& io_service);
  void write_to_socket(request_handler* handler);
  bool set_routes(std::map<std::string, std::shared_ptr<request_handler_factory>> route);
  path get_endpoint();
  bool set_request(bhttp::request<bhttp::dynamic_body> request);
  std::string match(std::map<std::string, std::shared_ptr<request_handler_factory>> routes, std::string url);
  void logIn() {profile.login_status = true;}
  void logOut() {profile.login_status = false;}
  bool update_user(std::string cookie_string, user_profile& user);
private:
  void log_info(std::string func_name, std::string message);
  void log_error(std::string func_name, std::string message);

  tcp::socket socket_;
  enum
  {
    max_length = 1024
  };
  char data_[max_length];
  boost::asio::ip::address dest_ip;
  http::server::request_parser req_parser;
  std::map<std::string, std::shared_ptr<request_handler_factory>> routes_;
  bhttp::request<bhttp::dynamic_body> request_;
  user_profile profile = {0, "", "", false};
};
