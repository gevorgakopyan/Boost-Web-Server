#include <fstream>
#include <stdio.h>

#include "gtest/gtest.h"
#include "profile_manager.h"

class profileManagerFixture : public ::testing::Test
{
  protected:
    const std::string directory = "../tests/";
    const std::string data_path = directory + "user_database.txt";
};

// Test for proper new user signup
TEST_F(profileManagerFixture, newUserSignUp)
{
  // Create/overwrite test database with one user.
  std::ofstream database_create(data_path);
  database_create << "1|test-user@example.com|test-user|N/A" << std::endl;
  database_create.close();

  // Attempt to sign up a new user.
  profile_manager manager(directory);
  bool success = manager.signup("new-user@example.com", "new-user", "new-password");

  EXPECT_TRUE(success);
}

// Test for attempt to signup for existing user.
TEST_F(profileManagerFixture, existingUserSignUp)
{
  // Create/overwrite test database with one user.
  std::ofstream database_create(data_path);
  database_create << "1|test-user@example.com|test-user|N/A" << std::endl;
  database_create.close();

  // Attempt to sign up a new user.
  profile_manager manager(directory);
  bool success = manager.signup("test-user@example.com", "test-user", "password");

  EXPECT_FALSE(success);
}

// Test for proper new user signup string
TEST_F(profileManagerFixture, goodSignupString)
{
  // Create/overwrite test database.
  std::ofstream database_create(data_path);
  database_create << "";
  database_create.close();

  // Attempt to sign up a new user.
  profile_manager manager(directory);
  bool success = manager.signup_string("new-user|new-password|new-user@example.com");

  EXPECT_TRUE(success);
}

// Test for malformed new user signup string
TEST_F(profileManagerFixture, badSignupString)
{
  // Create/overwrite test database.
  std::ofstream database_create(data_path);
  database_create << "";
  database_create.close();

  // Attempt to sign up a new user, should fail.
  profile_manager manager(directory);
  bool success = manager.signup_string("badsignupstring");

  EXPECT_FALSE(success);
}
TEST_F(profileManagerFixture, correctCrazyCodeCookie)
{
  std::ofstream database_create(data_path);
  database_create << "";
  database_create.close();
  profile_manager manager(directory);
  manager.signup("test-user@example.com", "test-user", "password");

  std::string updated_cookie = manager.update_cookie("crazyCode|test-user@example.com|password");
  bool success = (updated_cookie == "\"1|test-user|test-user@example.com\"");

  EXPECT_TRUE(success);
}

TEST_F(profileManagerFixture, malformedCookie)
{
  std::ofstream database_create(data_path);
  database_create << "1|1|test-user@example.com|test-user|N/A" << std::endl;
  database_create.close();

  profile_manager manager(directory);
  std::string updated_cookie = manager.update_cookie("badcookie");
  bool success = (updated_cookie == "");

  EXPECT_TRUE(success);
}

TEST_F(profileManagerFixture, loginProper)
{
  std::ofstream database_create(data_path);
  database_create << "";
  database_create.close();
  profile_manager manager(directory);
  manager.signup("test-user@example.com", "test-user", "password");

  user_profile profile;
  bool success = (manager.login("test-user@example.com", "password", profile) && 
                  profile.username == "test-user" &&
                  profile.email == "test-user@example.com");

  EXPECT_TRUE(success);
}


TEST_F(profileManagerFixture, badPassword)
{
  std::ofstream database_create(data_path);
  database_create << "";
  database_create.close();
  profile_manager manager(directory);
  manager.signup("test-user@example.com", "test-user", "password");

  user_profile profile;
  bool success = (manager.login("test-user@example.com", "incorrect-password", profile) && 
                  profile.user_id == -1);

  EXPECT_FALSE(success);
}
