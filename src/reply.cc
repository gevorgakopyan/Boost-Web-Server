#include "reply.h"

std::string reply::stock_reply(int err_code)
{
    std::string stock_reply;
    switch (err_code)
    {
      case 200:
        stock_reply = "";
        break;
      case 201:
        stock_reply = "<html>"
        "<head><title>Created</title></head>"
        "<body><h1>201 Created</h1></body>"
        "</html>";
        break;
      case 202:
        stock_reply = "<html>"
        "<head><title>Accepted</title></head>"
        "<body><h1>202 Accepted</h1></body>"
        "</html>";
        break;
      case 204:
        stock_reply = "<html>"
        "<head><title>No Content</title></head>"
        "<body><h1>204 Content</h1></body>"
        "</html>";
        break;
      case 300:
        stock_reply = "<html>"
        "<head><title>Multiple Choices</title></head>"
        "<body><h1>300 Multiple Choices</h1></body>"
        "</html>";
        break;
      case 301:
        stock_reply = "<html>"
        "<head><title>Moved Permanently</title></head>"
        "<body><h1>301 Moved Permanently</h1></body>"
        "</html>";
        break;
      case 302:
        stock_reply = "<html>"
        "<head><title>Moved Temporarily</title></head>"
        "<body><h1>302 Moved Temporarily</h1></body>"
        "</html>";
        break;
      case 304:
        stock_reply = "<html>"
        "<head><title>Not Modified</title></head>"
        "<body><h1>304 Not Modified</h1></body>"
        "</html>";
        break;
      case 400:
        stock_reply = "<html>"
        "<head><title>Bad Request</title></head>"
        "<body><h1>400 Bad Request</h1></body>"
        "</html>";
        break;
      case 401:
        stock_reply = "<html>"
        "<head><title>Unauthorized</title></head>"
        "<body><h1>401 Unauthorized</h1></body>"
        "</html>";
        break;
      case 403:
        stock_reply = "<html>"
        "<head><title>Forbidden</title></head>"
        "<body><h1>403 Forbidden</h1></body>"
        "</html>";
        break;
      case 404:
        stock_reply = "<html>"
        "<head><title>Not Found</title></head>"
        "<body><h1>404 Not Found</h1></body>"
        "</html>";
        break;
      case 500:
        stock_reply = "<html>"
        "<head><title>Internal Server Error</title></head>"
        "<body><h1>500 Internal Server Error</h1></body>"
        "</html>";
        break;
      case 501:
        stock_reply = "<html>"
        "<head><title>Not Implemented</title></head>"
        "<body><h1>501 Not Implemented</h1></body>"
        "</html>";
        break;
      case 502:
        stock_reply = "<html>"
        "<head><title>Bad Gateway</title></head>"
        "<body><h1>502 Bad Gateway</h1></body>"
        "</html>";
        break;
      case 503:
        stock_reply = "<html>"
        "<head><title>Service Unavailable</title></head>"
        "<body><h1>503 Service Unavailable</h1></body>"
        "</html>";
        break;
    }
    return stock_reply;
}

