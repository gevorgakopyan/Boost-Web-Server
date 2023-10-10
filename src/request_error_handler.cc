#include "request_error_handler.h"

request_error_handler::request_error_handler(bhttp::status ec): err_code(ec)
{

}

bhttp::status request_error_handler::handle_request(const bhttp::request<bhttp::dynamic_body> req, bhttp::response<bhttp::dynamic_body>& res)
{
  res.result(err_code);
  boost::beast::ostream(res.body()) << rep.stock_reply(res.result_int());
  res.content_length((res.body().size()));
  res.set(bhttp::field::content_type, "text/html");
  return err_code;
}