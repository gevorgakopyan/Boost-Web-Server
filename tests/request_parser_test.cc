#include "request.h"
#include "request_parser.h"
#include "gtest/gtest.h"

class RequestParserTest: public ::testing::Test
{
protected:
  http::server::request_parser parser;
  http::server::request request_;
  http::server::request_parser::parse_result result;
  http::server::request_parser::parse_result bad_result = http::server::request_parser::parse_result::bad;
  http::server::request_parser::parse_result good_result = http::server::request_parser::parse_result::good;
};

TEST_F(RequestParserTest, GoodRequest)
{
  char input[100] =
    "GET / HTTP/1.1\r\nHost: www.wesbite.com\r\nConnection: close\r\n\r\n";
  boost::tie(result, boost::tuples::ignore) =
    parser.parse(request_, input, input + strlen(input));
  EXPECT_EQ(result, good_result);
}

TEST_F(RequestParserTest, SlashMissingRequest)
{

  char input[100] =
    "GET HTTP/1.1\r\nHost: www.website.com\r\nConnection: close\r\n\r\n";
  boost::tie(result, boost::tuples::ignore) =
    parser.parse(request_, input, input + strlen(input));
  EXPECT_EQ(result, bad_result);
}

TEST_F(RequestParserTest, WrongProtocalRequest1)
{

  char input[100] =
    "GET / ATTP/1.1\r\nHost: www.website.com\r\nConnection: close\r\n\r\n";
  boost::tie(result, boost::tuples::ignore) =
    parser.parse(request_, input, input + strlen(input));
  EXPECT_EQ(result, bad_result);
}

TEST_F(RequestParserTest, WrongProtocalRequest2)
{

  char input[100] =
    "GET / HATP/1.1\r\nHost: www.website.com\r\nConnection: close\r\n\r\n";
  boost::tie(result, boost::tuples::ignore) =
    parser.parse(request_, input, input + strlen(input));
  EXPECT_EQ(result, bad_result);
}

TEST_F(RequestParserTest, WrongProtocalRequest3)
{

  char input[100] =
    "GET / HTAP/1.1\r\nHost: www.website.com\r\nConnection: close\r\n\r\n";
  boost::tie(result, boost::tuples::ignore) =
    parser.parse(request_, input, input + strlen(input));
  EXPECT_EQ(result, bad_result);
}

TEST_F(RequestParserTest, WrongProtocalRequest4)
{

  char input[100] =
    "GET / HTTA/1.1\r\nHost: www.website.com\r\nConnection: close\r\n\r\n";
  boost::tie(result, boost::tuples::ignore) =
    parser.parse(request_, input, input + strlen(input));
  EXPECT_EQ(result, bad_result);
}

TEST_F(RequestParserTest, BadVersionMinorRequest)
{

  char input[100] =
    "GET / HTTP/1.a\r\nHost: www.website.com\r\nConnection: close\r\n\r\n";
  boost::tie(result, boost::tuples::ignore) =
    parser.parse(request_, input, input + strlen(input));
  EXPECT_EQ(result, bad_result);
}

TEST_F(RequestParserTest, BadVersionMinorRequest2)
{

  char input[100] =
    "GET / HTTP/1.1a\r\nHost: www.website.com\r\nConnection: close\r\n\r\n";
  boost::tie(result, boost::tuples::ignore) =
    parser.parse(request_, input, input + strlen(input));
  EXPECT_EQ(result, bad_result);
}

TEST_F(RequestParserTest, BadVersionMajorRequest)
{

  char input[100] =
    "GET / HTTP/a.1\r\nHost: www.website.com\r\nConnection: close\r\n\r\n";
  boost::tie(result, boost::tuples::ignore) =
    parser.parse(request_, input, input + strlen(input));
  EXPECT_EQ(result, bad_result);
}

TEST_F(RequestParserTest, BadVersionMajorRequest2)
{

  char input[100] =
    "GET / HTTP/1a.1\r\nHost: www.website.com\r\nConnection: close\r\n\r\n";
  boost::tie(result, boost::tuples::ignore) =
    parser.parse(request_, input, input + strlen(input));
  EXPECT_EQ(result, bad_result);
}

TEST_F(RequestParserTest, WrongEndingAfterVersionRequest)
{

  char input[100] =
    "GET / HTTP/1.1\r Host: www.website.com\r\nConnection: close\r\n\r\n";
  boost::tie(result, boost::tuples::ignore) =
    parser.parse(request_, input, input + strlen(input));
  EXPECT_EQ(result, bad_result);
}

TEST_F(RequestParserTest, WrongEndingRequest)
{

  char input[100] =
    "GET / HTTP/1.1\r\nHost: www.website.com\r\nConnection: close\r\n\rend";
  boost::tie(result, boost::tuples::ignore) =
    parser.parse(request_, input, input + strlen(input));
  EXPECT_EQ(result, bad_result);
}

TEST_F(RequestParserTest, InvalidSpacesRequest)
{

  char input[100] = "GET / HTTP /1 . 1\r\n  Host: www.website.com \r\n\t  "
    "Type: test\r\n Connection: close\r\n\r\n";
  boost::tie(result, boost::tuples::ignore) =
    parser.parse(request_, input, input + strlen(input));
  EXPECT_EQ(result, bad_result);
}

TEST_F(RequestParserTest, InvalidHeaderRequest)
{

  char input[100] =
    "GET / HTTP/1.1\r\nHost:www.website.com\r\nConnection: close\r\n\r\n";
  boost::tie(result, boost::tuples::ignore) =
    parser.parse(request_, input, input + strlen(input));
  EXPECT_EQ(result, bad_result);
}

TEST_F(RequestParserTest, InvalidHeaderRequest2)
{

  char input[100] =
    "GET / HTTP/1.1\r\nHost www.website.com\r\nConnection: close\r\n\r\n";
  boost::tie(result, boost::tuples::ignore) =
    parser.parse(request_, input, input + strlen(input));
  EXPECT_EQ(result, bad_result);
}

TEST_F(RequestParserTest, ValidSpecialCharRequest)
{

  char input[100] = "GET / HTTP/1.1\r\nHost: www.website.com \r\nType: "
    "test()@#$%^&*!\r\n Connection: close\r\n\r\n";
  boost::tie(result, boost::tuples::ignore) =
    parser.parse(request_, input, input + strlen(input));
  EXPECT_EQ(result, good_result);
}

TEST_F(RequestParserTest, InvalidSpecialCharRequest)
{

  char input[100] = "GET / HTTP/1.1\r\nHost: www.website.com \r\nType: "
    "test\\r\n Connection: close\r\n\r\n";
  boost::tie(result, boost::tuples::ignore) =
    parser.parse(request_, input, input + strlen(input));
  EXPECT_EQ(result, bad_result);
}

TEST_F(RequestParserTest, IncompleteRequest)
{
  // incomplete request gives a result that is neither true nor false
  char input[100] =
    "GET / HTTP/1.1\r\nHost: www.website.com\r\nConnection: close\r\n";
  boost::tie(result, boost::tuples::ignore) =
    parser.parse(request_, input, input + strlen(input));
  EXPECT_EQ(result, http::server::request_parser::parse_result::indeterminate);
}

TEST_F(RequestParserTest, IncompleteRequest2)
{
  // incomplete request gives a result that is neither true nor false
  char input[100] =
    "GET / HTTP/1.1\r\nHost: www.website.com\r\nConnection: close";
  boost::tie(result, boost::tuples::ignore) =
    parser.parse(request_, input, input + strlen(input));
  EXPECT_EQ(result, http::server::request_parser::parse_result::indeterminate);
}

TEST_F(RequestParserTest, ParserValidRequest)
{

  char input[100] =
    "GET / HTTP/1.1\r\nHost: www.website.com\r\nConnection: close\r\n\r\n";
  boost::tie(result, boost::tuples::ignore) =
    parser.parse(request_, input, input + strlen(input));
  bool success1 = request_.method == "GET" && request_.uri == "/" &&
    request_.http_version_major == 1 &&
    request_.http_version_minor == 1;
  bool success2 = request_.headers[0].name == "Host" &&
    request_.headers[0].value == "www.website.com";
  bool success3 = request_.headers[1].name == "Connection" &&
    request_.headers[1].value == "close";
  EXPECT_TRUE(success1);
  EXPECT_TRUE(success2);
  EXPECT_TRUE(success3);
}

TEST_F(RequestParserTest, ParserInvalidRequest)
{

  char input[100] =
    "GET HTTP/1.1\r\nHost: www.website.com\r\nConnection: close\r\n\r\n";
  boost::tie(result, boost::tuples::ignore) =
    parser.parse(request_, input, input + strlen(input));
  bool success1 = request_.method == "GET" && request_.uri == "HTTP/1.1" &&
    //  request_.http_version_major == 1 && request_.http_version_minor == 1 &&
     request_.headers.empty();
    ;
  EXPECT_TRUE(success1);
}