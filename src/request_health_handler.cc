#include "request_health_handler.h"
#include <cstddef>
#include <string>
#include <vector>

request_health_handler::request_health_handler(std::string location, std::string request_url)
    : location_(location), request_url(request_url) {

}

bhttp::status request_health_handler::handle_request(const bhttp::request<bhttp::dynamic_body> req, bhttp::response<bhttp::dynamic_body>& res)
{
    std::string req_input = req.target().to_string();
    size_t last_slash_pos = req_input.find_last_not_of('/');
    if (last_slash_pos != std::string::npos) {
        req_input = req_input.substr(0, last_slash_pos + 1);
    }
    if (location_ != req_input)
    {
        res.result(bhttp::status::not_found);
        boost::beast::ostream(res.body()) << rep.stock_reply(res.result_int());
        res.content_length((res.body().size()));
        res.set(bhttp::field::content_type, "text/html");
        log_message_info("404");
        return res.result();
    }
    res.result(bhttp::status::ok);
    boost::beast::ostream(res.body()) << "OK";
    res.content_length((res.body().size()));
    res.set(bhttp::field::content_type, "text/plain");
    log_message_info("200");
    return res.result();
}

void request_health_handler::log_message_info(std::string res_code) {
    BOOST_LOG_TRIVIAL(info) << "[MetricsForResponse] Code for response: " << res_code << " URL for request: " << request_url << " Corresponding handler: health handler";
}
