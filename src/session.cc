#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/log/trivial.hpp>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <memory>

#include "request_echo_handler.h"
#include "request_error_handler.h"
#include "request_parser.h"
#include "request_static_handler.h"
#include "session.h"


using boost::asio::ip::tcp;

namespace bhttp = boost::beast::http;


session::session(boost::asio::io_service& io_service) : socket_(io_service) {}

tcp::socket& session::socket() { return socket_; }

bool session::set_request(bhttp::request<bhttp::dynamic_body> request)
{
  request_ = request;
  return true;
}
bool session::set_routes(std::map<std::string, std::shared_ptr<request_handler_factory>> routes)
{
  routes_ = routes;
  return true;
}

bool session::start() {
  boost::system::error_code error_code;
  auto remote_ep = socket_.remote_endpoint(error_code);
  dest_ip = error_code ? boost::asio::ip::address::from_string("127.0.0.1") : remote_ep.address(); // local host address
  log_info("start", "Accepting incoming requests.");

  bhttp::async_read(
      socket_, buf, request_,
      boost::bind(&session::handle_read, this, boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
  return true;
}

std::string session::handle_read(const boost::system::error_code& error, size_t bytes_transferred) {

  std::ostringstream ostring;
  ostring << request_;
  std::string request_string = ostring.str();
  if (!error) {
    log_info("handle_read", "request parser valid");
    update_user(std::string(request_[bhttp::field::cookie]), profile);
    std::string location = match(routes_, std::string(request_.target()));
    auto factory = routes_[location];
    request_handler* handler = factory->create(location, std::string(request_.target()), profile);
    write_to_socket(handler);
    delete handler;
  }
  else {
    log_error("handle_read", "request parser invalid");
    delete this;
  }

  return request_string;
}

std::string session::match(std::map<std::string, std::shared_ptr<request_handler_factory>> routes, std::string url)
{
  std::string matched_str = url.substr(0, url.length());
  size_t pos = url.length();
  while (pos != std::string::npos)
  {
    matched_str = matched_str.substr(0, pos);
    if (routes.find(matched_str) != routes.end())
    {
      return matched_str;
    }
    pos = matched_str.rfind('/');
  }
  return "/";
}

void session::write_to_socket(request_handler* req_h) {
  boost::system::error_code ec;
  socket_.remote_endpoint(ec);
  if (!ec) {
    bhttp::response <bhttp::dynamic_body> response_;
    req_h->handle_request(request_, response_);
    bhttp::write(socket_, response_);
    handle_write(boost::system::error_code());
  }
  else {
    BOOST_LOG_TRIVIAL(error) << "Unable to write to socket. Error code: " << boost::system::system_error(ec).what();
    return;
  }
}

bool session::handle_write(const boost::system::error_code& error) {
  if (!error) {
    boost::system::error_code ignored_err;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_err);
    log_info("handle_write", "connection closed successfully");
    return true;
  }
  else {
    log_error("handle_write", "error closing the connection");
    delete this;
    return false;
  }
}

session_interface* session::get_session(boost::asio::io_service& io_service) {
  return new session(io_service);
}

bool session::update_user(std::string cookie_string, user_profile& user)
{
  std::string cookies = ";" + cookie_string;
  for(auto param : bhttp::param_list(cookies))
  {
    if (param.first == "crazyCode")
    {
      std::vector<std::string> info;
      std::string data = std::string(param.second);

      int begin = 0;
      int end = data.find("|", begin);
      while (end != std::string::npos) {
          info.push_back(data.substr(begin, end - begin));
          begin = end+1;
          end = data.find("|", begin);
      }
      if (begin < data.size())
      {
        info.push_back(data.substr(begin, data.size()));
      }

      if (info.size() != 3)
      {
        return false;
      }

      user.user_id = std::stoi(info.at(0));
      user.username = info.at(1);
      user.email = info.at(2);
      user.login_status = true;

      return true;
    }
  }
  return false;
}

void session::log_info(std::string func_name, std::string message) {
  BOOST_LOG_TRIVIAL(info) << "Client IP: " << dest_ip.to_string() << "\tRequest url: " << std::string(request_.target()) << "\tsession::" << func_name << ":\t" << message;
}

void session::log_error(std::string func_name, std::string message) {
  BOOST_LOG_TRIVIAL(error) << "Client IP: " << dest_ip.to_string() << "\tRequest url: " << std::string(request_.target()) << "\tsession::" << func_name << ":\t" << message;
}