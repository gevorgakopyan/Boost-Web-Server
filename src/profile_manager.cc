#include "profile_manager.h"
#include <boost/log/trivial.hpp>
/**
 * Generate a profile manager that will interact with a database at a
 * specified location.
 *
 * Profiles will now be stored in the following format:
 * id | email | name | password
 */
profile_manager::profile_manager(std::string path) :
  directory_path(path) { }

/**
 * Processes data received from server and puts it in an
 * easily parsable format.
 */
bool profile_manager::signup_string(std::string data)
{
  std::vector<std::string> info;

  // Extract user info from signup string.
  int begin = 0;
  int end = data.find("|", begin);
  while (end != std::string::npos)
  {
    info.push_back(data.substr(begin, end - begin));
    begin = end + 1;
    end = data.find("|", begin);
  }
  if (begin < data.size())
  {
    info.push_back(data.substr(begin, data.size()));
  }

  // Check malformed request size
  if (info.size() != 3)
  {
    return false;
  }

  return signup(info.at(2), info.at(0), info.at(1));
}

bool profile_manager::login(std::string email, std::string password, user_profile& profile)
{

  const std::string data_path = directory_path + "user_database.txt";
  boost::filesystem::path database_path(data_path);

  if (boost::filesystem::exists(database_path))
  {
    std::string line;
    std::ifstream database_in(data_path);
    while (std::getline(database_in, line))
    {
      std::string::size_type id_end = line.find('|');
      if (id_end == std::string::npos) continue;

      std::string::size_type email_end = line.find('|', id_end + 1);
      if (email_end == std::string::npos) continue;

      std::string::size_type name_end = line.find('|', email_end + 1);
      if (email_end == std::string::npos) continue;

      std::string::size_type password_end = line.find('|', name_end + 1);

      std::string::size_type email_length = (email_end - id_end) - 1;
      std::string::size_type name_length = (name_end - email_end) - 1;
      if (line.substr(id_end + 1, email_length) == email)
      {
        if (hash_pass(password) == line.substr(name_end + 1, 64)) // SHA-256 always 64 characters.
        {
          profile = { std::stoi(line.substr(0, id_end)),
                     line.substr(email_end + 1, name_length),
                     line.substr(id_end + 1, email_length),
                     true };
          database_in.close();
          return true;
        }
        else
        {
          database_in.close();
          profile = { -1, "", "", false };
          return false;
        }
      }
    }
    profile = { -1, "Email address not present within database", "", false };
    return false;
  }
  profile = { -1, "Database file does not exist", "", false };
  return false;
}

/**
 * Checks if an account is already using the passed in email in the database.
*/
bool profile_manager::email_exists(std::string email) {
  const std::string data_path = directory_path + "user_database.txt";
  boost::filesystem::path database_path(data_path);

  if (boost::filesystem::exists(database_path)) {
    std::string line;
    std::ifstream database_in(data_path);

    while (std::getline(database_in, line)) {
      std::string::size_type id_end = line.find('|');
      if (id_end == std::string::npos) continue;
      std::string::size_type email_end = line.find('|', id_end + 1);
      std::string db_email = line.substr(id_end + 1, email_end - id_end - 1);
      if (email == db_email)
        return true;
    }
  }
  return false;
}

  /**
   * Signups for new users. Will return false if user already exists in database
   * (emails must be unique), or if some of the fields provided are invalid.
   */
bool profile_manager::signup(std::string email, std::string name, std::string password)
{
  // Keep track of users detected, intialized to zero.
  int num_users = 0;
  int user_id;

  const std::string data_path = directory_path + "user_database.txt";
  boost::filesystem::path database_path(data_path);

  if (boost::filesystem::exists(database_path))
  {
    std::string line;
    std::ifstream database_in(data_path);

    // Check every line of the database for the email address.
    while (std::getline(database_in, line))
    {
      std::string::size_type id_end = line.find('|');
      std::string::size_type email_end = line.find('|', id_end + 1);

      if (id_end != std::string::npos)
      {
        // Increment the number of users.
        num_users++;

        // Compare line substring containing email address.
        std::string::size_type email_length = (email_end - id_end) - 1;
        if (line.substr(id_end + 1, email_length) == email)
        {
          // User already exists in the database.
          BOOST_LOG_TRIVIAL(info) << "User: " << email << " already present in database";
          database_in.close();
          return false;
        }
      }
    }

    database_in.close();

    // User ID reflects newest user.
    user_id = num_users + 1;

    std::string filename(data_path);
    std::ofstream database_out;

    database_out.open(filename, std::ios_base::app);

    // Write to database with hashed password.
    database_out << user_id
      << "|" << email
      << "|" << name
      << "|" << hash_pass(password)
      << std::endl;

    database_out.close();

    std::string message = "User: " + email +
      " successfully added to database with ID: " + std::to_string(user_id);

    BOOST_LOG_TRIVIAL(info) << message;
    return true;
  }
  else
  {
    std::ofstream database_create(data_path);
    database_create << 1
      << "|" << email
      << "|" << name
      << "|" << hash_pass(password)
      << std::endl;

    database_create.close();

    return true;
  }
}

/**
 * Returns the hashed value of the password that will be stored in the database.
 * The database is a text file.
 * Also used for determining if correct passwords are used during login.
 * Implements SHA256 hashing to accomplish this.
 */
std::string profile_manager::hash_pass(std::string password)
{
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, password.c_str(), password.size());
  SHA256_Final(hash, &sha256);
  std::stringstream hashed_password;
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
  {
    hashed_password << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
  }
  return hashed_password.str();
}

std::string profile_manager::update_cookie(std::string cookie_string)
{
  std::string updated_data;

  std::vector<std::string> info;
  std::string data = cookie_string;

  // Extract user info from cookie.
  int begin = 0;
  int end = data.find("|", begin);
  while (end != std::string::npos)
  {
    info.push_back(data.substr(begin, end - begin));
    begin = end + 1;
    end = data.find("|", begin);
  }
  if (begin < data.size())
  {
    info.push_back(data.substr(begin, data.size()));
  }

  // Check malformed request size
  if (info.size() != 3)
  {
    return updated_data;
  }

  // Check for type of login
  if (info.at(0) == "crazyCode")
  {
    user_profile found_user;
    // Check for user existence
    if (login(info.at(1), info.at(2), found_user))
    {
      updated_data = "\"" + std::to_string(found_user.user_id) + "|" +
        found_user.username + "|" +
        found_user.email + "\"";
    }
  }

  return updated_data;
}