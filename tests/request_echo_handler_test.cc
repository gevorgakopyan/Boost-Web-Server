#include <cstring>
#include <iostream>
#include <boost/asio/buffers_iterator.hpp>

#include "gtest/gtest.h"
#include "request_echo_handler.h"

TEST(EchoHandlerTest, EmptyRequest)
{
  bhttp::request<bhttp::dynamic_body> request;  
  request.target("/echo");

  std::ostringstream ostring;
  ostring << request;
  std::string request_string = ostring.str();

  bhttp::response<bhttp::dynamic_body> response;
  request_echo_handler echo_handler("/echo", "test url");
  bhttp::status status = echo_handler.handle_request(request, response);

  std::string body { boost::asio::buffers_begin(response.body().data()),
                     boost::asio::buffers_end(response.body().data()) };

  std::vector<std::pair<std::string, std::string>> headers;
  for(auto const& field : response)
  {
    std::pair<std::string, std::string> header;
    header.first = std::string(field.name_string());
    header.second = std::string(field.value());
    headers.push_back(header);
  }

  bool success = (
    response.result() == bhttp::status::ok &&
    body == request_string &&
    response.has_content_length() &&
    headers.at(1).first == "Content-Type" &&
    headers.at(1).second == "text/plain" &&
    status == response.result());

  EXPECT_TRUE(success);
}

TEST(EchoHandlerTest, NormalRequest)
{
  bhttp::request<bhttp::dynamic_body> request;  
  request.target("/echo");
  boost::beast::ostream(request.body()) << "normal request";

  std::ostringstream ostring;
  ostring << request;
  std::string request_string = ostring.str();

  bhttp::response<bhttp::dynamic_body> response;
  request_echo_handler echo_handler("/echo", "test url");
  bhttp::status status = echo_handler.handle_request(request, response);

  std::string body { boost::asio::buffers_begin(response.body().data()),
                     boost::asio::buffers_end(response.body().data()) };

  std::vector<std::pair<std::string, std::string>> headers;
  for(auto const& field : response)
  {
    std::pair<std::string, std::string> header;
    header.first = std::string(field.name_string());
    header.second = std::string(field.value());
    headers.push_back(header);
  }

  bool success = (response.result() == bhttp::status::ok &&
                  body == request_string &&
                  response.has_content_length() && 
                  headers.at(1).first == "Content-Type" &&
                  headers.at(1).second == "text/plain" &&
                  status == response.result());

  EXPECT_TRUE(success);
}

TEST(EchoHandlerTest, BadRequest)
{
  bhttp::request<bhttp::dynamic_body> request;  
  request.target("/echo/bad");
  boost::beast::ostream(request.body()) << "bad request";

  std::ostringstream ostring;
  ostring << request;
  std::string request_string = ostring.str();

  bhttp::response<bhttp::dynamic_body> response;
  request_echo_handler echo_handler("/echo", "test url");
  bhttp::status status = echo_handler.handle_request(request, response);

  std::string body { boost::asio::buffers_begin(response.body().data()),
                     boost::asio::buffers_end(response.body().data()) };

  std::vector<std::pair<std::string, std::string>> headers;
  for(auto const& field : response)
  {
    std::pair<std::string, std::string> header;
    header.first = std::string(field.name_string());
    header.second = std::string(field.value());
    headers.push_back(header);
  }

  bool success = (response.result() == bhttp::status::not_found && 
                  status == response.result());

  EXPECT_TRUE(success);
}