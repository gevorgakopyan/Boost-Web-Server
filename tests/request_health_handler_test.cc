#include <cstring>
#include <boost/asio/buffers_iterator.hpp>

#include "gtest/gtest.h"
#include "request_health_handler.h"

TEST(healthHandlerTest, normalRequest)
{
  bhttp::request<bhttp::dynamic_body> request_;  
  request_.target("/health");

  bhttp::response<bhttp::dynamic_body> answer;
  request_health_handler handler("/health", "test url");
  bhttp::status status_ = handler.handle_request(request_, answer);

  std::string body { boost::asio::buffers_begin(answer.body().data()),
                    boost::asio::buffers_end(answer.body().data()) };

  std::vector<std::pair<std::string, std::string>> headers;
  for(auto const& field : answer) {
      headers.emplace_back(std::string(field.name_string()), std::string(field.value()));
  }

  bool isStatusOK = answer.result() == bhttp::status::ok;
  bool isBodyOK = body == "OK";
  bool hasContentLength = answer.has_content_length();
  bool isContentTypeValid = false;
  if (headers.size() > 1 && headers[1].first == "Content-Type" && headers[1].second == "text/plain") {
      isContentTypeValid = true;
  }
  bool isStatusMatching = status_ == answer.result();

  EXPECT_TRUE(isStatusOK);
  EXPECT_TRUE(isBodyOK);
  EXPECT_TRUE(hasContentLength);
  EXPECT_TRUE(isContentTypeValid);
  EXPECT_TRUE(isStatusMatching);
}

TEST(healthHandlerTest, badRequestTarget)
{
  bhttp::request<bhttp::dynamic_body> request_;  
  request_.target("/health/bad");
  boost::beast::ostream(request_.body()) << "test request";

  std::ostringstream ostring;
  ostring << request_;
  std::string request_string = ostring.str();

  bhttp::response<bhttp::dynamic_body> answer;
  request_health_handler handler("/health", "test url");
  bhttp::status status_ = handler.handle_request(request_, answer);

  std::string body { boost::asio::buffers_begin(answer.body().data()),
                    boost::asio::buffers_end(answer.body().data()) };

  std::vector<std::pair<std::string, std::string>> headers;
  for(auto const& field : answer) {
      headers.emplace_back(std::string(field.name_string()), std::string(field.value()));
  }

  bool isStatusNotFound = answer.result() == bhttp::status::not_found;
  bool isStatusMatching = status_ == answer.result();

  EXPECT_TRUE(isStatusNotFound);
  EXPECT_TRUE(isStatusMatching);
}