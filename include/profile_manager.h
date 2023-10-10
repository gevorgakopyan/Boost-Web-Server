#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <openssl/sha.h>
#include <sstream>
#include "user_profile.h"

#include "boost/filesystem.hpp"

namespace fs = boost::filesystem;

/**
 * Allows the creation of user profiles in a database (text file)
 * and creates a corresponding user id with each new user.
 */
class profile_manager
{
public:
  profile_manager(std::string path);
  bool email_exists(std::string email);
  bool signup(std::string email, std::string name, std::string password);
  bool signup_string(std::string data);
  bool login(std::string email, std::string password, user_profile& profile);
  std::string update_cookie(std::string cookie_string);

private:
  std::string hash_pass(std::string password);
  std::string directory_path;
};