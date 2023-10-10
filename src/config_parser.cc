// An nginx config file parser.
//
// See:
//   http://wiki.nginx.org/Configuration
//   http://blog.martinfjordvald.com/2010/07/nginx-primer/
//
// How Nginx does it:
//   http://lxr.nginx.org/source/src/core/ngx_conf_file.c

#include <boost/log/trivial.hpp>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

#include "config_parser.h"
#include "constants.h"

std::string NginxConfig::ToString(int depth)
{
  std::string serialized_config;
  for (const auto& statement : statements_)
  {
    serialized_config.append(statement->ToString(depth));
  }
  return serialized_config;
}

int NginxConfig::get_listen_port() {
  int port = -1;
  for (auto s : statements_) {
    if (s->child_block_.get() != nullptr) {
      port = s->child_block_->get_listen_port();
    }
    else {
      if (s->tokens_.size() == 2 && s->tokens_[0] == "listen") {
        port = stoi(s->tokens_[1]);
        break;
      }
    }
  }
  if (port < 1 || port > 65535) {
    throw std::invalid_argument("Port " + std::to_string(port) +
                                " out of range");
  }
  return port;
}

std::string NginxConfig::get_root(std::string location) {
  for (path p : paths) {
    if (p.endpoint != location && (p.type != endpoint_type::static_ || p.type != endpoint_type::api)) continue;
    return p.root;
  }
  return "";
}

std::map<std::string, std::string> NginxConfig::get_auth_path_map(std::string location) {
  for (path p : paths) {
    if (p.endpoint != location && (p.type != endpoint_type::auth)) continue;
    return p.config_key_to_path_map;
  }
  std::map<std::string, std::string> emptyMap;
  return emptyMap;
}

std::vector<path> NginxConfig::get_paths() {
  for (auto s : statements_) {
    if (s->tokens_[0] != kUrlPathKeyword) {
      continue;
    }
    if (s->tokens_[2] == kStaticHandler) {
      path static_path;
      for (auto child_statement : s->child_block_->statements_) {
        if (child_statement->tokens_[0] == kResourcePathKeyword) {
          static_path.type = static_;
          static_path.endpoint = s->tokens_[1];
          size_t last_slash_pos = static_path.endpoint.find_last_not_of('/');
          if (last_slash_pos != std::string::npos) {
            static_path.endpoint = static_path.endpoint.substr(0, last_slash_pos + 1);
          }
          static_path.root = child_statement->tokens_[1];
        }
      }
      paths.push_back(static_path);
    }
    else if (s->tokens_[2] == kEchoHandler) {
      path echo_path;
      echo_path.type = echo;
      echo_path.endpoint = s->tokens_[1];
      size_t last_slash_pos = echo_path.endpoint.find_last_not_of('/');
      if (last_slash_pos != std::string::npos) {
        echo_path.endpoint = echo_path.endpoint.substr(0, last_slash_pos + 1);
      }
      paths.push_back(echo_path);
    }
    else if (s->tokens_[2] == kAPIHandler) {
        path api_path; 
        for (auto child_statement : s->child_block_->statements_) {
            if (child_statement->tokens_[0] == kResourcePathKeyword) {
            api_path.type = api;
            api_path.endpoint = s->tokens_[1];
            size_t last_slash_pos = api_path.endpoint.find_last_not_of('/');
            if (last_slash_pos != std::string::npos) {
                api_path.endpoint = api_path.endpoint.substr(0, last_slash_pos + 1);
            }
            api_path.root = child_statement->tokens_[1];
        }
      }
      paths.push_back(api_path);
    }
    else if (s->tokens_[2] == kHealthHandler){
      path health_path;
      health_path.type = health;
      health_path.endpoint = s->tokens_[1];
      size_t last_slash_pos = health_path.endpoint.find_last_not_of('/');
      if (last_slash_pos != std::string::npos) {
        health_path.endpoint = health_path.endpoint.substr(0, last_slash_pos + 1);
      }
      paths.push_back(health_path);

    }
    else if (s->tokens_[2] == kSleepHandler){
      path sleep_path;
      sleep_path.type = sleep_;
      sleep_path.endpoint = s->tokens_[1];
      size_t last_slash_pos = sleep_path.endpoint.find_last_not_of('/');
      if (last_slash_pos != std::string::npos) {
        sleep_path.endpoint = sleep_path.endpoint.substr(0, last_slash_pos + 1);
      }
      paths.push_back(sleep_path);

    }
    else if (s->tokens_[2] == kAuthenticationHandler)
      {
        path auth_path;
        auth_path.type = auth;
        for (const std::shared_ptr<NginxConfigStatement> child_statement : s->child_block_->statements_)
        {
          // Locate login, logout, signup statements with corresponding sub-endpoints
          if (child_statement->tokens_.size() == 2)
          {
            auth_path.type = auth;
            auth_path.endpoint = s->tokens_[1];
            size_t last_slash_pos = auth_path.endpoint.find_last_not_of('/');
            if (last_slash_pos != std::string::npos) {
              auth_path.endpoint = auth_path.endpoint.substr(0, last_slash_pos + 1);
            }
            auth_path.config_key_to_path_map[child_statement->tokens_[0]] = child_statement->tokens_[1];
            auth_path.root = auth_path.config_key_to_path_map["root"];
          }
        }
        paths.push_back(auth_path);
      }
    else if (s->tokens_[2] == "ApiConsoleHandler")
      {
          for (const std::shared_ptr<NginxConfigStatement> child_statement : s->child_block_->statements_)
        {
          // Locate "data_path" statements with corresponding path
          if (child_statement->tokens_.size() == 2)
          {
            path console_path;
            console_path.type = console;
            console_path.endpoint = s->tokens_[1];
            console_path.config_key_to_path_map[child_statement->tokens_[0]] = child_statement->tokens_[1];
            console_path.root = console_path.config_key_to_path_map["root"];
            paths.push_back(console_path);
          }
        }
      }
  }

  for (auto p : paths) {
    BOOST_LOG_TRIVIAL(info) << "path endpoint_type " << p.type;
    BOOST_LOG_TRIVIAL(info) << "path endpoint: " << p.endpoint;
    BOOST_LOG_TRIVIAL(info) << "path root: " << p.root;
  }

  return paths;
}

std::string NginxConfigStatement::ToString(int depth) {
  std::string serialized_statement;
  for (int i = 0; i < depth; ++i) {
    serialized_statement.append("  ");
  }
  for (unsigned int i = 0; i < tokens_.size(); ++i) {
    if (i != 0) {
      serialized_statement.append(" ");
    }
    serialized_statement.append(tokens_[i]);
  }
  if (child_block_.get() != nullptr) {
    serialized_statement.append(" {\n");
    serialized_statement.append(child_block_->ToString(depth + 1));
    for (int i = 0; i < depth; ++i) {
      serialized_statement.append("  ");
    }
    serialized_statement.append("}");
  }
  else {
    serialized_statement.append(";");
  }
  serialized_statement.append("\n");
  return serialized_statement;
}

const char* NginxConfigParser::TokenTypeAsString(TokenType type)
{
  switch (type)
  {
    case TOKEN_TYPE_START:
      return "TOKEN_TYPE_START";
    case TOKEN_TYPE_NORMAL:
      return "TOKEN_TYPE_NORMAL";
    case TOKEN_TYPE_START_BLOCK:
      return "TOKEN_TYPE_START_BLOCK";
    case TOKEN_TYPE_END_BLOCK:
      return "TOKEN_TYPE_END_BLOCK";
    case TOKEN_TYPE_COMMENT:
      return "TOKEN_TYPE_COMMENT";
    case TOKEN_TYPE_STATEMENT_END:
      return "TOKEN_TYPE_STATEMENT_END";
    case TOKEN_TYPE_EOF:
      return "TOKEN_TYPE_EOF";
    case TOKEN_TYPE_ERROR:
      return "TOKEN_TYPE_ERROR";
    default:
      return "Unknown token type";
  }
}

NginxConfigParser::TokenType NginxConfigParser::ParseToken(std::istream* input,
                                                           std::string* value) {
  TokenParserState state = TOKEN_STATE_INITIAL_WHITESPACE;
  while (input->good()) {
    const char c = input->get();
    if (!input->good()) {
      break;
    }
    switch (state) {
      case TOKEN_STATE_INITIAL_WHITESPACE:
        switch (c) {
          case '{':
            *value = c;
            return TOKEN_TYPE_START_BLOCK;
          case '}':
            *value = c;
            return TOKEN_TYPE_END_BLOCK;
          case '#':
            *value = c;
            state = TOKEN_STATE_TOKEN_TYPE_COMMENT;
            continue;
          case '"':
            *value = c;
            state = TOKEN_STATE_DOUBLE_QUOTE;
            continue;
          case '\'':
            *value = c;
            state = TOKEN_STATE_SINGLE_QUOTE;
            continue;
          case ';':
            *value = c;
            return TOKEN_TYPE_STATEMENT_END;
          case ' ':
          case '\t':
          case '\n':
          case '\r':
            continue;
          default:
            *value += c;
            state = TOKEN_STATE_TOKEN_TYPE_NORMAL;
            continue;
        }
      case TOKEN_STATE_SINGLE_QUOTE:
        *value += c;
        if (c == '\'') {
          const char next_c = input->get();
          if (next_c == ' ' || next_c == '\n' || next_c == '\t' ||
              next_c == '{' || next_c == '}' || next_c == ';') {
            input->unget();
          }
          else {
            return TOKEN_TYPE_ERROR;
          }
          return TOKEN_TYPE_NORMAL;
        }
        else if (c == '\\') {
          value->pop_back();
          state = TOKEN_STATE_ESCAPE_SINGLE_QUOTE;
        }
        continue;
      case TOKEN_STATE_DOUBLE_QUOTE:
        *value += c;
        if (c == '"') {
          const char next_c = input->get();
          if (next_c == ' ' || next_c == '\n' || next_c == '\t' ||
              next_c == '{' || next_c == '}' || next_c == ';') {
            input->unget();
          }
          else {
            return TOKEN_TYPE_ERROR;
          }
          return TOKEN_TYPE_NORMAL;
        }
        else if (c == '\\') {
          value->pop_back();
          state = TOKEN_STATE_ESCAPE_DOUBLE_QUOTE;
        }
        continue;
      case TOKEN_STATE_TOKEN_TYPE_COMMENT:
        if (c == '\n' || c == '\r') {
          return TOKEN_TYPE_COMMENT;
        }
        *value += c;
        continue;
      case TOKEN_STATE_TOKEN_TYPE_NORMAL:
        if (c == ' ' || c == '\t' || c == '\n' || c == '\t' || c == ';' ||
            c == '{' || c == '}') {
          input->unget();
          return TOKEN_TYPE_NORMAL;
        }
        *value += c;
        continue;
      case TOKEN_STATE_ESCAPE_SINGLE_QUOTE:
        *value += c;
        state = TOKEN_STATE_SINGLE_QUOTE;
        continue;
      case TOKEN_STATE_ESCAPE_DOUBLE_QUOTE:
        *value += c;
        state = TOKEN_STATE_DOUBLE_QUOTE;
        continue;
    }
  }

  // If we get here, we reached the end of the file.
  if (state == TOKEN_STATE_SINGLE_QUOTE || state == TOKEN_STATE_DOUBLE_QUOTE) {
    return TOKEN_TYPE_ERROR;
  }

  return TOKEN_TYPE_EOF;
}

bool NginxConfigParser::Parse(std::istream* config_file, NginxConfig* config) {
  std::stack<NginxConfig*> config_stack;
  config_stack.push(config);
  TokenType last_token_type = TOKEN_TYPE_START;
  TokenType token_type;
  while (true) {
    std::string token;
    token_type = ParseToken(config_file, &token);
    BOOST_LOG_TRIVIAL(info)
      << TokenTypeAsString(token_type) << ": " << token.c_str();
    if (token_type == TOKEN_TYPE_ERROR) {
      break;
    }

    if (token_type == TOKEN_TYPE_COMMENT) {
      // Skip comments.
      continue;
    }

    if (token_type == TOKEN_TYPE_START) {
      // Error.
      break;
    }
    else if (token_type == TOKEN_TYPE_NORMAL) {
      if (last_token_type == TOKEN_TYPE_START ||
          last_token_type == TOKEN_TYPE_STATEMENT_END ||
          last_token_type == TOKEN_TYPE_START_BLOCK ||
          last_token_type == TOKEN_TYPE_END_BLOCK ||
          last_token_type == TOKEN_TYPE_NORMAL) {
        if (last_token_type != TOKEN_TYPE_NORMAL) {
          config_stack.top()->statements_.emplace_back(
              new NginxConfigStatement);
        }
        config_stack.top()->statements_.back().get()->tokens_.push_back(token);
      }
      else {
     // Error.
        break;
      }
    }
    else if (token_type == TOKEN_TYPE_STATEMENT_END) {
      if (last_token_type != TOKEN_TYPE_NORMAL) {
        // Error.
        break;
      }
    }
    else if (token_type == TOKEN_TYPE_START_BLOCK) {
      if (last_token_type != TOKEN_TYPE_NORMAL) {
        // Error.
        break;
      }
      NginxConfig* const new_config = new NginxConfig;
      config_stack.top()->statements_.back().get()->child_block_.reset(
          new_config);
      config_stack.push(new_config);
    }
    else if (token_type == TOKEN_TYPE_END_BLOCK) {
      if (last_token_type != TOKEN_TYPE_START_BLOCK &&
          last_token_type != TOKEN_TYPE_STATEMENT_END &&
          last_token_type != TOKEN_TYPE_END_BLOCK) {
        // Error.
        break;
      }
      for (auto s : config_stack.top()->statements_) {
        if (s->tokens_[0] == "listen") {
          if (s->tokens_.size() != 2) {
            break;
          }
        }
      }
      config_stack.pop();
    }
    else if (token_type == TOKEN_TYPE_EOF) {
      if (last_token_type != TOKEN_TYPE_STATEMENT_END &&
          last_token_type != TOKEN_TYPE_END_BLOCK &&
          last_token_type != TOKEN_TYPE_START) {
        // Error.
        break;
      }
      config_stack.pop();
      if (config_stack.size() > 0) {
        break;
      }
      return true;
    }
    else {
   // Error. Unknown token.
      break;
    }
    last_token_type = token_type;
  }

  BOOST_LOG_TRIVIAL(error) << "Bad transition from "
    << TokenTypeAsString(last_token_type) << " to "
    << TokenTypeAsString(token_type);
  return false;
}

bool NginxConfigParser::Parse(const char* file_name, NginxConfig* config) {
  std::ifstream config_file;
  config_file.open(file_name);
  if (!config_file.good()) {
    BOOST_LOG_TRIVIAL(error) << "Failed to open config file: " << file_name;
    return false;
  }

  const bool return_value =
    Parse(dynamic_cast<std::istream*>(&config_file), config);
  config_file.close();
  return return_value;
}
