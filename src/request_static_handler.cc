#include "request_static_handler.h"
#include "mime_types.h"

request_static_handler::request_static_handler(std::string location, std::string root, std::string url) :
  location_(location), root_(root), request_url(url)
{

}

bhttp::status request_static_handler::handle_request(const bhttp::request<bhttp::dynamic_body> req, bhttp::response<bhttp::dynamic_body>& res)
{
  std::string req_input = req.target().to_string();
  size_t last_slash_pos = req_input.find_last_not_of('/');
  if (last_slash_pos != std::string::npos) {
    req_input = req_input.substr(0, last_slash_pos + 1);
  }

  std::string file_name = req_input.substr(location_.size(), std::string::npos);
  size_t ext_start = file_name.find_last_of(".");
  if (ext_start != std::string::npos)
  {
    extension = file_name.substr(ext_start + 1);
  }

  std::string full_path = root_ + "/" + file_name;

  boost::filesystem::path boost_path(full_path);
  if (!boost::filesystem::exists(boost_path) || !boost::filesystem::is_regular_file(full_path))
  {
    res.result(bhttp::status::not_found);
    boost::beast::ostream(res.body()) << rep.stock_reply(res.result_int());
    res.content_length((res.body().size()));
    res.set(bhttp::field::content_type, "text/html");
    log_message_info("404");
    return res.result();
  }

  std::ifstream file(full_path.c_str(), std::ios::in | std::ios::binary);
  if (!file)
  {
    res.result(bhttp::status::not_found);
    boost::beast::ostream(res.body()) << rep.stock_reply(res.result_int());
    res.content_length((res.body().size()));
    res.set(bhttp::field::content_type, "text/html");
    log_message_info("404");
    return res.result();
  }

  char c;
  std::string reply_body;
  while (file.get(c))
  {
    reply_body += c;
  }
  file.close();

  res.result(bhttp::status::ok);
  boost::beast::ostream(res.body()) << reply_body;
  res.content_length((res.body().size()));
  res.set(bhttp::field::content_type, mime_types::extension_to_type(extension));
  log_message_info("200");
  return res.result();
}

void request_static_handler::log_message_info(std::string res_code)
{
  BOOST_LOG_TRIVIAL(info) << "[MetricsForResponse] Code for response: " << res_code << " URL for request: " << request_url << " Corresponding handler: static handler";
}