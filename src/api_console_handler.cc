#include <boost/beast.hpp>
#include <boost/log/trivial.hpp>
#include <cstddef>
#include <iostream>
#include <string>

#include "profile_manager.h"
#include "api_console_handler.h"

api_console_handler::api_console_handler(std::string location, std::string request_url, std::string root, user_profile profile) 
  : location_(location), request_url_(request_url), root_(root), profile_(profile) {

}

bhttp::status api_console_handler::handle_request(const bhttp::request<bhttp::dynamic_body> req, bhttp::response<bhttp::dynamic_body>& res)
{
  if (profile_.login_status == false)
  {
    res.result(bhttp::status::unauthorized);
    boost::beast::ostream(res.body()) << rep_.stock_reply(res.result_int());
    res.content_length((res.body().size()));
    res.set(bhttp::field::content_type, "text/html");
    return res.result();
  }

  res.result(bhttp::status::ok);
  std::string console_page;
  // read in signup page
    std::string path = root_ + "/console.html";
    fs::path file_path(path);
    if (exists(file_path) && is_regular_file(file_path))
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
        console_page = temp_data;
      }
    }
  boost::beast::ostream(res.body()) << console_page;
  res.content_length((res.body().size()));
  res.set(bhttp::field::content_type, "text/html");
  
  return res.result();
}