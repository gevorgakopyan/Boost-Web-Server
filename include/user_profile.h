#pragma once

#include <iostream>
#include <string>

/**
 * A user profile struct will contain user
 * information and login status.
**/
struct user_profile
{
  int user_id; 
  std::string username;
  std::string email;
  bool login_status;
};