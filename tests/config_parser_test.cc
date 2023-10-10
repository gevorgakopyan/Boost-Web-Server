#include "config_parser.h"
#include "gtest/gtest.h"

class NginxConfigParserTest : public ::testing::Test
{
protected:
  void SetUp() override { return; }
  NginxConfigParser parser;
  NginxConfig out_config;
};

TEST_F(NginxConfigParserTest, SimpleConfig)
{
  bool success = parser.Parse("example_config", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, CommentConfig)
{
  bool success = parser.Parse("example_config_2", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, EmptyConfig)
{
  bool success = parser.Parse("example_config_3", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, MultipleConfig)
{
  bool success = parser.Parse("example_config_4", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, NestedConfig)
{
  bool success = parser.Parse("example_config_5", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, BackSlashConfig)
{
  bool success = parser.Parse("example_config_6", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, MissingBlockConfig)
{
  bool fail = parser.Parse("example_config_7", &out_config);
  EXPECT_FALSE(fail);
}

TEST_F(NginxConfigParserTest, DoubleSemiColonConfig)
{
  bool fail = parser.Parse("example_config_8", &out_config);
  EXPECT_FALSE(fail);
}

TEST_F(NginxConfigParserTest, MissingNameBlockConfig)
{
  bool fail = parser.Parse("example_config_9", &out_config);
  EXPECT_FALSE(fail);
}

TEST_F(NginxConfigParserTest, MissingSemiColonConfig)
{
  bool fail = parser.Parse("example_config_10", &out_config);
  EXPECT_FALSE(fail);
}

TEST_F(NginxConfigParserTest, MissingClosingBraceConfig)
{
  bool fail = parser.Parse("example_config_11", &out_config);
  EXPECT_FALSE(fail);
}

TEST_F(NginxConfigParserTest, ExtraClosingBraceConfig)
{
  bool fail = parser.Parse("example_config_12", &out_config);
  EXPECT_FALSE(fail);
}

TEST_F(NginxConfigParserTest, SingleQuoteBackslashEscape)
{
  bool success = parser.Parse("example_config_13", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, DoubleQuoteBackslashEscape)
{
  bool success = parser.Parse("example_config_14", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, NoWhiteSpaceAfterQuote)
{
  bool fail = parser.Parse("example_config_15", &out_config);
  EXPECT_FALSE(fail);
}

TEST_F(NginxConfigParserTest, ToString) {
  bool success = parser.Parse("test_config", &out_config);
  std::string expected_config_string = "server {\n  listen 80;\n  echo {\n    location /echo {\n    }\n  }\n}\n";
  EXPECT_EQ(out_config.ToString(0), expected_config_string);
}

TEST_F(NginxConfigParserTest, GetListenPort) {
  bool success = parser.Parse("test_config", &out_config);
  EXPECT_EQ(out_config.get_listen_port(), 80);
}

TEST_F(NginxConfigParserTest, GetListenPortOutOfRange) {
  bool success = parser.Parse("outofrange_config", &out_config);
  EXPECT_THROW(out_config.get_listen_port(), std::invalid_argument);
}

TEST_F(NginxConfigParserTest, GetPaths) {
  bool success = parser.Parse("paths_config", &out_config);
  std::vector<path> configured_paths = {
    path{endpoint_type::static_, "/static1", "../static_files/static1"},
    path{endpoint_type::static_, "/static2", "../static_files/static2"},
    path{endpoint_type::static_, "/static3", "../static_files/static3"},
    path{endpoint_type::echo, "/echo", ""},
    path{endpoint_type::echo, "/echo2", ""},
  };
  std::vector<path> retrieved_paths = out_config.get_paths();
  ASSERT_EQ(configured_paths.size(), retrieved_paths.size());
  for (int i = 0; i < configured_paths.size(); ++i) {
    EXPECT_EQ(retrieved_paths[i].type, configured_paths[i].type);
    EXPECT_EQ(retrieved_paths[i].endpoint, configured_paths[i].endpoint);
    EXPECT_EQ(retrieved_paths[i].root, configured_paths[i].root);
  }
}