#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <vector>
#include "reply.h"
#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>
#include <boost/log/trivial.hpp>
#include "user_profile.h"

namespace bhttp = boost::beast::http;

class request_handler
{
  public:
    virtual bhttp::status handle_request(const bhttp::request<bhttp::dynamic_body> req, bhttp::response<bhttp::dynamic_body>& res) = 0;
    virtual user_profile get_profile() {return user_profile{0, "", "", false};};
    virtual ~request_handler() {}
};

#endif