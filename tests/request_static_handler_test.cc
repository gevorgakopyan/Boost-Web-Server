#include "boost/filesystem.hpp"
#include <boost/asio/buffers_iterator.hpp>
#include "gtest/gtest.h"
#include "request_static_handler.h"

class StaticHandlerFixture : public :: testing::Test
{
  protected:
    std::string root = "../static_files/static1";
    std::string base_uri = "/static1/";
};

TEST_F(StaticHandlerFixture, NormalHTMLFileResponseTest)
{
  std::string filename = "example.html";
  bhttp::request<bhttp::dynamic_body> request;  
  request.target(base_uri + filename);

  bhttp::response<bhttp::dynamic_body> response;
  request_static_handler static_handler(base_uri, root, request.target().to_string());
  bhttp::status status = static_handler.handle_request(request, response);

  std::string full_path = root + "/" + filename;
  std::ifstream file_(full_path.c_str(), std::ios::in | std::ios::binary);

  char c;
  std::string expected = "";
  while (file_.get(c))
  {
    expected += c;
  }
  file_.close();

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
                  body == expected &&
                  response.has_content_length() &&
                  headers.at(0).first == "Content-Length" &&
                  headers.at(0).second == std::to_string(expected.size()) &&
                  headers.at(1).first == "Content-Type" &&
                  headers.at(1).second == "text/html" &&
                  status == response.result());

  EXPECT_TRUE(success);
}

TEST_F(StaticHandlerFixture, NormalTxtFileResponseTest)
{
  std::string filename = "example.txt";
  bhttp::request<bhttp::dynamic_body> request;  
  request.target(base_uri + filename);

  bhttp::response<bhttp::dynamic_body> response;
  request_static_handler static_handler(base_uri, root, request.target().to_string());
  bhttp::status status = static_handler.handle_request(request, response);

  std::string full_path = root + "/" + filename;
  std::ifstream file_(full_path.c_str(), std::ios::in | std::ios::binary);

  char c;
  std::string expected = "";
  while (file_.get(c))
  {
    expected += c;
  }
  file_.close();

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
                  body == expected &&
                  response.has_content_length() &&
                  headers.at(0).first == "Content-Length" &&
                  headers.at(0).second == std::to_string(expected.size()) &&
                  headers.at(1).first == "Content-Type" &&
                  headers.at(1).second == "text/plain" &&
                  status == response.result());

  EXPECT_TRUE(success);
}

TEST_F(StaticHandlerFixture, BadFileResponseTest)
{
  std::string filename = "non_existant.html";
  bhttp::request<bhttp::dynamic_body> request;  
  request.target(base_uri + filename);

  bhttp::response<bhttp::dynamic_body> response;
  request_static_handler static_handler(base_uri, root, request.target().to_string());
  bhttp::status status = static_handler.handle_request(request, response);

  bool success = (response.result() == bhttp::status::not_found && 
                  status == response.result());

  EXPECT_TRUE(success);
}