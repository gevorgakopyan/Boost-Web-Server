#include "gtest/gtest.h"
#include "mime_types.h"

class mimeTypesFixture : public ::testing::Test {};

TEST_F(mimeTypesFixture, GifTest)
{
  std::string extension = "gif";
  bool success = (mime_types::extension_to_type(extension) == "image/gif");
  EXPECT_TRUE(success);
}

TEST_F(mimeTypesFixture, HtmTest)
{
  std::string extension = "htm";
  bool success = (mime_types::extension_to_type(extension) == "text/html");
  EXPECT_TRUE(success);
}

TEST_F(mimeTypesFixture, HtmlTest)
{
  std::string extension = "html";
  bool success = (mime_types::extension_to_type(extension) == "text/html");
  EXPECT_TRUE(success);
}

TEST_F(mimeTypesFixture, JpgTest)
{
  std::string extension = "jpg";
  bool success = (mime_types::extension_to_type(extension) == "image/jpeg");
  EXPECT_TRUE(success);
}

TEST_F(mimeTypesFixture, JpegTest)
{
  std::string extension = "jpeg";
  bool success = (mime_types::extension_to_type(extension) == "image/jpeg");
  EXPECT_TRUE(success);
}

TEST_F(mimeTypesFixture, PngTest)
{
  std::string extension = "png";
  bool success = (mime_types::extension_to_type(extension) == "image/png");
  EXPECT_TRUE(success);
}

TEST_F(mimeTypesFixture, txtTest)
{
  std::string extension = "txt";
  bool success = (mime_types::extension_to_type(extension) == "text/plain");
  EXPECT_TRUE(success);
}

TEST_F(mimeTypesFixture, ZipTest)
{
  std::string extension = "zip";
  bool success = (mime_types::extension_to_type(extension) == "application/zip");
  EXPECT_TRUE(success);
}

TEST_F(mimeTypesFixture, DefaultTest)
{
  std::string extension = "default extension";
  bool success = (mime_types::extension_to_type(extension) == "text/plain");
  EXPECT_TRUE(success);
}