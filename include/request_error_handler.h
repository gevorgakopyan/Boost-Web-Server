#ifndef REQUEST_ERROR_HANDLER_H
#define REQUEST_ERROR_HANDLER_H

#include "request_handler.h"

class request_error_handler: public request_handler
{
public:
  request_error_handler(bhttp::status error_code);
  bhttp::status handle_request(const bhttp::request<bhttp::dynamic_body> req, bhttp::response<bhttp::dynamic_body>& res);

private:
  bhttp::status err_code;
  reply rep;
};

#endif