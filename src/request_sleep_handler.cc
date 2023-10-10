#include "request_sleep_handler.h"
#include <cstddef>
#include <string>
#include <vector>


request_sleep_handler::request_sleep_handler(std::string location, std::string request_url)
  : location_(location), request_url(request_url) {

}

bhttp::status request_sleep_handler::handle_request(const bhttp::request<bhttp::dynamic_body> req, bhttp::response<bhttp::dynamic_body>& res)
{
  usleep(2000000);
  std::string input = req.target().to_string();
  size_t last_slash_pos = input.find_last_not_of('/');
  if (last_slash_pos != std::string::npos) {
    input = input.substr(0, last_slash_pos + 1);
  }
  res.result(bhttp::status::ok);
  boost::beast::ostream(res.body()) << "This request slept: " + std::to_string(200000) + " microseconds";
  res.content_length((res.body().size()));
  res.set(bhttp::field::content_type, "text/plain");

  log_message_info("200");

  return res.result();
}

void request_sleep_handler::log_message_info(std::string res_code)
{
  BOOST_LOG_TRIVIAL(info) << "[MetricsForResponse] Code for response: " << res_code << " URL for request: " << request_url << " Corresponding handler: sleep handler";
}