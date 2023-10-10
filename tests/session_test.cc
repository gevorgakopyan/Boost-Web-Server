#include "session.h"
#include "gtest/gtest.h"

class SessionFixture : public ::testing::Test
{
protected:
  boost::asio::io_service io_service;
};

TEST_F(SessionFixture, SessionStart)
{
  session s(io_service);
  EXPECT_TRUE(s.start());
}

TEST_F(SessionFixture, SetRequest)
{
  session s(io_service);

}

TEST_F(SessionFixture, TestHasLongestMatchingPref)
{
  session s(io_service);
  std::map<std::string, std::shared_ptr<request_handler_factory>> routes;
  routes.emplace("/static2", nullptr);
  EXPECT_EQ(s.match(routes, "/static2/testing.txt"), "/static2");
}

TEST_F(SessionFixture, TestHasNoLongestMatchingPref)
{
  session s(io_service);
  std::map<std::string, std::shared_ptr<request_handler_factory>> routes;
  routes.emplace("/static1", nullptr);
  EXPECT_EQ(s.match(routes, "/static2/testing.txt"), "/");
}
