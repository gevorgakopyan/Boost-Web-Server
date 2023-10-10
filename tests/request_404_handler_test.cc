#include <boost/asio/buffers_iterator.hpp>
#include <cstring>
#include "gtest/gtest.h"
#include <iostream>
#include "request_404_handler.h"


TEST(Request404HandlerTest, NotFoundRequest)
{
  bhttp::request<bhttp::dynamic_body> request;

  bhttp::response<bhttp::dynamic_body> response;
  request_404_handler* error_handler = new request_404_handler("/", "test url");
  bhttp::status status = error_handler->handle_request(request, response);

  std::string body { boost::asio::buffers_begin(response.body().data()),
    boost::asio::buffers_end(response.body().data()) };

  std::vector<std::pair<std::string, std::string>> headers;
  for (auto const& field : response)
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

// Previously had an issue where 2 back to back 404 handler calls would cause a
// segmentation fault. This test is to ensure that the bug fix continues to work.
TEST(Request404HandlerTest, DoubleNotFoundRequest)
{
  bhttp::request<bhttp::dynamic_body> request;

  bhttp::response<bhttp::dynamic_body> response;
  request_404_handler* error_handler = new request_404_handler("/", "test url");
  bhttp::status status = error_handler->handle_request(request, response);

  std::string body { boost::asio::buffers_begin(response.body().data()),
    boost::asio::buffers_end(response.body().data()) };

  std::vector<std::pair<std::string, std::string>> headers;
  for (auto const& field : response)
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

  bhttp::request<bhttp::dynamic_body> request2;

  bhttp::response<bhttp::dynamic_body> response2;
  request_404_handler* error_handler2 = new request_404_handler("/", "test url");
  bhttp::status status2 = error_handler2->handle_request(request2, response2);

  std::string body2 { boost::asio::buffers_begin(response2.body().data()),
    boost::asio::buffers_end(response2.body().data()) };

  std::vector<std::pair<std::string, std::string>> headers2;
  for (auto const& field2 : response2)
  {
    std::pair<std::string, std::string> header2;
    header2.first = std::string(field2.name_string());
    header2.second = std::string(field2.value());
    headers2.push_back(header2);
  }

  reply rep2;
  std::string expected_body2 = rep2.stock_reply(404);

  bool success2 = (response2.result() == bhttp::status::not_found &&
                  body2 == expected_body2 &&
                  response2.has_content_length() &&
                  headers2.at(0).first == "Content-Length" &&
                  headers2.at(0).second == std::to_string(expected_body2.size()) &&
                  headers2.at(1).first == "Content-Type" &&
                  headers2.at(1).second == "text/html" &&
                  status2 == response2.result());

  EXPECT_TRUE(success2);
}