#ifndef REQUEST_API_HANDLER_H
#define REQUEST_API_HANDLER_H

#include "request_handler.h"

class request_api_handler : public request_handler
{
public:
  request_api_handler(std::string location, std::string root, std::string url, std::map<std::string, std::vector<int>>& path_counts, user_profile profile);
  bhttp::status handle_request(const bhttp::request<bhttp::dynamic_body> req, bhttp::response<bhttp::dynamic_body>& res);
private:
  void log_message_info(std::string res_code);

  std::string location_;
  std::string root_;
  std::string request_url;
  reply rep;
  std::map<std::string, std::vector<int>>& path_counts;
  user_profile profile_;

  int getNextID(std::string directory);
  bhttp::status handle_post(bhttp::request<bhttp::dynamic_body> req, bhttp::response<bhttp::dynamic_body>& res, std::string directory);
  bhttp::status handle_delete(bhttp::request<bhttp::dynamic_body> req, bhttp::response<bhttp::dynamic_body>& res, std::string directory);
  bhttp::status handle_get(bhttp::request<bhttp::dynamic_body> req, bhttp::response<bhttp::dynamic_body>& res, std::string directory);
  bhttp::status handle_list(bhttp::request<bhttp::dynamic_body> req, bhttp::response<bhttp::dynamic_body>& res, std::string directory);
  bhttp::status handle_put(bhttp::request<bhttp::dynamic_body> req, bhttp::response<bhttp::dynamic_body>& res, std::string directory);
  bhttp::status send_bad_request(bhttp::response<bhttp::dynamic_body>& res);
};

#endif