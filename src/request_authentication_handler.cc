#include <boost/beast.hpp>
#include <boost/log/trivial.hpp>
#include <cstddef>
#include <iostream>
#include <string>
#include <Poco/Net/SMTPClientSession.h>
#include <Poco/Net/MailMessage.h>

#include "profile_manager.h"
#include "request_authentication_handler.h"


request_authentication_handler::request_authentication_handler(std::string location, std::string request_url, std::string root, std::string data_path, std::string signup, std::string login, std::string logout, user_profile profile)
  : location_(location), request_url_(request_url), root_(root), data_path_(data_path), signup_(signup), login_(login), logout_(logout), profile_(profile) {

}


bhttp::status request_authentication_handler::handle_request(const bhttp::request<bhttp::dynamic_body> req, bhttp::response<bhttp::dynamic_body>& res)
{
  /**
   * Handle the request and check for signup
   * TODO: still need to checkout if the user is trying to login and then subsequently logout.
   */
  std::string input = req.target().to_string();
  if (location_ + login_ == input)
  {
    res.result(bhttp::status::ok);
    std::string login_page;

    std::string path = root_ + "/" + "login.html";
    fs::path filePath(path);
    if (exists(filePath) && is_regular_file(filePath))
    {
      // read file
      std::ifstream f(path);
      std::string temp_data;
      char c;
      if (f.is_open())
      {
        while (f.get(c))
        {
          temp_data += c;
        }
        f.close();
        login_page = temp_data;
      }
    }
    boost::beast::ostream(res.body()) << login_page;

    res.content_length((res.body().size()));
    res.set(bhttp::field::content_type, "text/html");
  }
  else if (location_ + login_ + "-submit" == input)
  {
    std::string data = boost::beast::buffers_to_string(req.body().data());

    // set cookie for our webserver
    profile_manager manager(data_path_ + "/");
    std::string new_cookie = manager.update_cookie(data);
    // check that cookie is created
    if (new_cookie != "")
    {
      res.set(bhttp::field::set_cookie, "crazyCode=" + new_cookie + "; Path=/");
      res.result(bhttp::status::ok);
    }
    else
    {
      res.result(bhttp::status::bad_request);
      res.content_length((res.body().size()));
    }

  }
  // check for logout sub-handler
  else if (location_ + logout_ == input)
  {
    std::string data = boost::beast::buffers_to_string(req.body().data());
    // set cookie for our webserver
    res.set(bhttp::field::set_cookie, "crazyCode=; Path=/; Max-Age=0");
    res.result(bhttp::status::ok);
  }
  else if (location_ + signup_ == input)
  {
    res.result(bhttp::status::ok);
    std::string signup_page;

    // read in signup page
    std::string path = root_ + "/" + "signup.html";
    fs::path filePath(path);
    if (exists(filePath) && is_regular_file(filePath))
    {
      // read file
      std::ifstream f(path);
      std::string temp_data;
      char c;
      if (f.is_open())
      {
        while (f.get(c))
        {
          temp_data += c;
        }
        f.close();
        signup_page = temp_data;
      }
    }
    boost::beast::ostream(res.body()) << signup_page;

    res.content_length((res.body().size()));
    res.set(bhttp::field::content_type, "text/html");
  }
  // check for signup-submit sub-handler
  else if (location_ + signup_ + "-submit" == input)
  {
    std::string data = boost::beast::buffers_to_string(req.body().data());

    profile_manager manager(data_path_ + "/");
    // check for succeessful signup
    if (!manager.signup_string(data))
    {
      res.result(bhttp::status::bad_request);
      boost::beast::ostream(res.body()) << rep.stock_reply(res.result_int());
      res.content_length((res.body().size()));
      res.set(bhttp::field::content_type, "text/html");
    }
    else
    {
      boost::beast::ostream(res.body()) << "OK";
      res.content_length((res.body().size()));
      res.result(bhttp::status::ok);
    }
  }
  else if (location_ + signup_ + "-send-verification" == input) {
    profile_manager manager(data_path_ + "/");
    Poco::Net::SMTPClientSession session("smtp.mailjet.com", 587);
    std::string apiKey = "1f1f7e34da21aed2a40279ffe41265cd";
    std::string apiSecret = "88ec141f976233dec73318feab585270";
    session.login(Poco::Net::SMTPClientSession::AUTH_LOGIN, apiKey, apiSecret);

    std::string data = boost::beast::buffers_to_string(req.body().data());
    std::string::size_type email_end = data.find('|');
    if (email_end == std::string::npos) {
      res.result(bhttp::status::bad_request);
      return res.result();
    }
    std::string::size_type username_end = data.find('|', email_end + 1);
    if (username_end == std::string::npos) {
      res.result(bhttp::status::bad_request);
      return res.result();
    }
    std::string::size_type verification_code_end = data.find('|', username_end + 1);

    std::string email = data.substr(0, email_end);
    std::string username = data.substr(email_end + 1, username_end - email_end - 1);
    std::string verification_code = data.substr(username_end + 1, verification_code_end - username_end - 1);

    if (manager.email_exists(email)) {
      res.result(bhttp::status::bad_request);
      boost::beast::ostream(res.body()) << rep.stock_reply(res.result_int());
      res.content_length((res.body().size()));
      res.set(bhttp::field::content_type, "text/html");
    }
    else {
      // Create the email message
      Poco::Net::MailMessage message;
      message.setSender("eninjabot@gmail.com");
      message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT, email));
      message.setSubject("Crazy Code Verification");
      message.setContent("Hello " + username + ",\n\nPlease enter the following verification code in the Crazy Code website:\n\n" + verification_code);

      // Send the email
      try {
        session.sendMessage(message);
        res.result(bhttp::status::ok);
        BOOST_LOG_TRIVIAL(info) << "Send email successful. Recpipient info => Email: " << email << " Username: " << username << " Verification Code: " << verification_code;
      }
      catch (Poco::Exception& ex) {
        res.result(bhttp::status::bad_request);
        BOOST_LOG_TRIVIAL(error) << "Send email unsuccessful. Error encountered: " << ex.displayText();
      }
    }

    // Close the SMTP client session
    session.close();
  }
  // if no matching sub-handler found, return 404
  else
  {
    res.result(bhttp::status::not_found);
    boost::beast::ostream(res.body()) << rep.stock_reply(res.result_int());
    res.content_length((res.body().size()));
    res.set(bhttp::field::content_type, "text/html");

    log_message_info("404");
  }

  return res.result();
}

user_profile request_authentication_handler::get_profile()
{
  return profile_;
}

void request_authentication_handler::log_message_info(std::string res_code)
{
  BOOST_LOG_TRIVIAL(info) << "[ResponseMetrics] "
    << "response_code: "
    << res_code
    << " "
    << "request_path: "
    << request_url_
    << " "
    << "matched_handler: authentication handler";
}