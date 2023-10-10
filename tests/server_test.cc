#include "mock_session.h"
#include "server.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

class ServerTest : public ::testing::Test
{
protected:
  mock_session mock_sesh;
  short port = 8080;
  boost::asio::io_service io_service;
};

TEST_F(ServerTest, StartServer)
{
  session s(io_service);

  server serv(s, io_service, port);
  NginxConfig config;
  NginxConfigParser parser;
  parser.Parse("../configs/test_config", &config);
  serv.init_server(config);

  EXPECT_TRUE(serv.start_accept());
}

// Test mock session with no socket defintion
TEST_F(ServerTest, StartBadServer)
{
  // Session.start() should not be called with no socket
  EXPECT_CALL(mock_sesh, start()).Times(0);

  server serv(mock_sesh, io_service, port);

  EXPECT_FALSE(serv.start_accept());
}

// Test mock session with valid session object
TEST_F(ServerTest, ServerAccept)
{
  mock_session test_mock;

  // Check session.start() is called once
  EXPECT_CALL(test_mock, start).Times(1);

  // Ensure test session mock is only returned once
  EXPECT_CALL(mock_sesh, get_session)
    .WillOnce(testing::Return(&test_mock))
    .WillRepeatedly(testing::Return((session_interface*)NULL));

  server serv(mock_sesh, io_service, port);

  EXPECT_TRUE(serv.handle_accept(mock_sesh.get_session(io_service),
    boost::system::error_code()));
}