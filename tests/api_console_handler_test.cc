#include <cstring>
#include <boost/asio/buffers_iterator.hpp>
#include "gtest/gtest.h"
#include "api_console_handler.h"

class ApiConsoleHandlerFixture : public :: testing::Test
{
  protected:
    bhttp::request<bhttp::dynamic_body> request_;
    bhttp::response<bhttp::dynamic_body> response_;
};

// Test for Api Console request body
TEST_F(ApiConsoleHandlerFixture, normalRequest)
{
  user_profile profile = {0, "", "", true};
  request_.target("/console");
  api_console_handler console_handler("/console", "test url", "test", profile);
  bhttp::status status_ = console_handler.handle_request(request_, response_);
  std::string body { boost::asio::buffers_begin(response_.body().data()),
                     boost::asio::buffers_end(response_.body().data()) };
  std::vector<std::pair<std::string, std::string>> headers;
  for(auto const& field : response_)
  {
    std::pair<std::string, std::string> header;
    header.first = std::string(field.name_string());
    header.second = std::string(field.value());
    headers.push_back(header);
  }

  bool success = (response_.result() == bhttp::status::ok &&
                  response_.has_content_length() &&
                  headers.at(1).first == "Content-Type" &&
                  headers.at(1).second == "text/html" &&
                  status_ == response_.result());
  EXPECT_TRUE(success);
}

// Test for not logged in to fail
TEST_F(ApiConsoleHandlerFixture, badRequest)
{
  user_profile profile = {0, "", "", false};
  request_.target("/console");
  api_console_handler console_handler("/console", "test url", "test", profile);
  bhttp::status status_ = console_handler.handle_request(request_, response_);
  std::string body { boost::asio::buffers_begin(response_.body().data()),
                     boost::asio::buffers_end(response_.body().data()) };
  std::vector<std::pair<std::string, std::string>> headers;
  for(auto const& field : response_)
  {
    std::pair<std::string, std::string> header;
    header.first = std::string(field.name_string());
    header.second = std::string(field.value());
    headers.push_back(header);
  }

  bool success = (response_.result() == bhttp::status::unauthorized &&
                  response_.has_content_length() &&
                  headers.at(1).first == "Content-Type" &&
                  headers.at(1).second == "text/html" &&
                  status_ == response_.result());
  EXPECT_TRUE(success);
}