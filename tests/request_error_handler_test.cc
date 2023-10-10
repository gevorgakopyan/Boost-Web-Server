#include <cstring>
#include <iostream>
#include "gtest/gtest.h"
#include "request_error_handler.h"
#include <boost/asio/buffers_iterator.hpp>

TEST(ErrorHandlerTest, BadRequest)
{
  bhttp::request<bhttp::dynamic_body> request;  

  bhttp::response<bhttp::dynamic_body> response;
  request_error_handler error_handler(bhttp::status::bad_request);
  bhttp::status status = error_handler.handle_request(request, response);

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
  
  reply rep;
  std::string expected_body = rep.stock_reply(400);

  bool success = (response.result() == bhttp::status::bad_request &&
                  body == expected_body &&
                  response.has_content_length() &&
                  headers.at(0).first == "Content-Length" &&
                  headers.at(0).second == std::to_string(expected_body.size()) &&
                  headers.at(1).first == "Content-Type" &&
                  headers.at(1).second == "text/html" &&
                  status == response.result());

  EXPECT_TRUE(success);
}

TEST(ErrorHandlerTest, NotFoundRequest)
{
  bhttp::request<bhttp::dynamic_body> request;  

  bhttp::response<bhttp::dynamic_body> response;
  request_error_handler error_handler(bhttp::status::not_found);
  bhttp::status status = error_handler.handle_request(request, response);

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
  
  reply rep;
  std::string expected_body = rep.stock_reply(404);

  bool success = (response.result() == bhttp::status::not_found &&
                  body == expected_body &&
                  response.has_content_length() &&
                  headers.at(0).first == "Content-Length" &&
                  headers.at(0).second == std::to_string(expected_body.size()) &&
                  headers.at(1).first == "Content-Type" &&
                  headers.at(1).second == "text/html" &&
                  status == response.result());

  EXPECT_TRUE(success);
}