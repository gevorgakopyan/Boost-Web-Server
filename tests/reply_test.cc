#include "reply.h"
#include "gtest/gtest.h"

class ReplyTest: public ::testing::Test
{
protected:
  reply rep;
};

TEST_F(ReplyTest, OkReply)
{
  std::string expected_string = "";
  bool success = rep.stock_reply(200) == expected_string;

  EXPECT_TRUE(success);
}

TEST_F(ReplyTest, CreatedReply)
{
  std::string expected_string = "<html>"
        "<head><title>Created</title></head>"
        "<body><h1>201 Created</h1></body>"
        "</html>";
  bool success = rep.stock_reply(201) == expected_string;

  EXPECT_TRUE(success);
}

TEST_F(ReplyTest, AcceptedReply)
{
  std::string expected_string = "<html>"
        "<head><title>Accepted</title></head>"
        "<body><h1>202 Accepted</h1></body>"
        "</html>";
  bool success = rep.stock_reply(202) == expected_string;

  EXPECT_TRUE(success);
}

TEST_F(ReplyTest, NoContentReply)
{
  std::string expected_string = "<html>"
        "<head><title>No Content</title></head>"
        "<body><h1>204 Content</h1></body>"
        "</html>";
  bool success = rep.stock_reply(204) == expected_string;

  EXPECT_TRUE(success);
}

TEST_F(ReplyTest, MultipleChoicesReply)
{
  std::string expected_string = "<html>"
        "<head><title>Multiple Choices</title></head>"
        "<body><h1>300 Multiple Choices</h1></body>"
        "</html>";
  bool success = rep.stock_reply(300) == expected_string;

  EXPECT_TRUE(success);
}

TEST_F(ReplyTest, MovedPermanently)
{
  std::string expected_string = "<html>"
        "<head><title>Moved Permanently</title></head>"
        "<body><h1>301 Moved Permanently</h1></body>"
        "</html>";
  bool success = rep.stock_reply(301) == expected_string;

  EXPECT_TRUE(success);
}

TEST_F(ReplyTest, MovedTemporarilyReply)
{
  std::string expected_string = "<html>"
        "<head><title>Moved Temporarily</title></head>"
        "<body><h1>302 Moved Temporarily</h1></body>"
        "</html>";
  bool success = rep.stock_reply(302) == expected_string;

  EXPECT_TRUE(success);
}

TEST_F(ReplyTest, NotModifiedReply)
{
  std::string expected_string = "<html>"
        "<head><title>Not Modified</title></head>"
        "<body><h1>304 Not Modified</h1></body>"
        "</html>";
  bool success = rep.stock_reply(304) == expected_string;

  EXPECT_TRUE(success);
}

TEST_F(ReplyTest, BadRequestReply)
{
  std::string expected_string = "<html>"
        "<head><title>Bad Request</title></head>"
        "<body><h1>400 Bad Request</h1></body>"
        "</html>";
  bool success = rep.stock_reply(400) == expected_string;

  EXPECT_TRUE(success);
}

TEST_F(ReplyTest, UnauthorizedReply)
{
  std::string expected_string = "<html>"
        "<head><title>Unauthorized</title></head>"
        "<body><h1>401 Unauthorized</h1></body>"
        "</html>";
  bool success = rep.stock_reply(401) == expected_string;

  EXPECT_TRUE(success);
}

TEST_F(ReplyTest, ForbiddenReply)
{
  std::string expected_string = "<html>"
        "<head><title>Forbidden</title></head>"
        "<body><h1>403 Forbidden</h1></body>"
        "</html>";
  bool success = rep.stock_reply(403) == expected_string;

  EXPECT_TRUE(success);
}

TEST_F(ReplyTest, NotFoundReply)
{
  std::string expected_string = "<html>"
        "<head><title>Not Found</title></head>"
        "<body><h1>404 Not Found</h1></body>"
        "</html>";
  bool success = rep.stock_reply(404) == expected_string;

  EXPECT_TRUE(success);
}

TEST_F(ReplyTest, InternalServerErrorReply)
{
  std::string expected_string = "<html>"
        "<head><title>Internal Server Error</title></head>"
        "<body><h1>500 Internal Server Error</h1></body>"
        "</html>";
  bool success = rep.stock_reply(500) == expected_string;

  EXPECT_TRUE(success);
}

TEST_F(ReplyTest, NotImplementedReply)
{
  std::string expected_string = "<html>"
        "<head><title>Not Implemented</title></head>"
        "<body><h1>501 Not Implemented</h1></body>"
        "</html>";
  bool success = rep.stock_reply(501) == expected_string;

  EXPECT_TRUE(success);
}

TEST_F(ReplyTest, BadGatewayReply)
{
  std::string expected_string = "<html>"
        "<head><title>Bad Gateway</title></head>"
        "<body><h1>502 Bad Gateway</h1></body>"
        "</html>";
  bool success = rep.stock_reply(502) == expected_string;

  EXPECT_TRUE(success);
}

TEST_F(ReplyTest, ServiceUnavailableReply)
{
  std::string expected_string = "<html>"
        "<head><title>Service Unavailable</title></head>"
        "<body><h1>503 Service Unavailable</h1></body>"
        "</html>";
  bool success = rep.stock_reply(503) == expected_string;

  EXPECT_TRUE(success);
}
