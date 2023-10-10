#include <cstring>
#include <boost/asio/buffers_iterator.hpp>
#include "gtest/gtest.h"
#include "request_sleep_handler.h"

TEST(sleepHandlerTest, normalRequest)
{
  bhttp::request<bhttp::dynamic_body> request_;  
  request_.target("/sleep");
  bhttp::response<bhttp::dynamic_body> answer;
  request_sleep_handler handler("/sleep", "test url");
  bhttp::status status_ = handler.handle_request(request_, answer);
  std::string body { boost::asio::buffers_begin(answer.body().data()),
                     boost::asio::buffers_end(answer.body().data()) };
  std::vector<std::pair<std::string, std::string>> headers;
  for(auto const& field : answer)
  {
    std::pair<std::string, std::string> header;
    header.first = std::string(field.name_string());
    header.second = std::string(field.value());
    headers.push_back(header);
  }
  EXPECT_TRUE(answer.result() == bhttp::status::ok);
  EXPECT_TRUE(body == "This request slept: " + std::to_string(200000) + " microseconds");
  EXPECT_TRUE(answer.has_content_length());
  EXPECT_TRUE(headers.at(1).first == "Content-Type");
  EXPECT_TRUE(headers.at(1).second == "text/plain");
  }
TEST(sleepHandlerTest, badRequest)
{
  bhttp::request<bhttp::dynamic_body> request_;  
  request_.target("/sleep");
  bhttp::response<bhttp::dynamic_body> answer;
  request_sleep_handler handler("/sleep", "test url");
  bhttp::status status_ = handler.handle_request(request_, answer);
  std::string body { boost::asio::buffers_begin(answer.body().data()),
                     boost::asio::buffers_end(answer.body().data()) };
  std::vector<std::pair<std::string, std::string>> headers;
  for(auto const& field : answer)
  {
    std::pair<std::string, std::string> header;
    header.first = std::string(field.name_string());
    header.second = std::string(field.value());
    headers.push_back(header);
  }
  EXPECT_FALSE(body == "This request slept: " + std::to_string(20022200) + " microseconds");
  EXPECT_TRUE(answer.has_content_length());
  EXPECT_TRUE(headers.at(1).first == "Content-Type");
  EXPECT_TRUE(headers.at(1).second == "text/plain");
}