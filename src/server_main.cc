//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "config_parser.h"
#include "server.h"
#include "session.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/core.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/filesystem.hpp>

using boost::asio::ip::tcp;

namespace logging = boost::log;
namespace keywords = boost::log::keywords;

// Signal handler for keyboard interrupt
// Called when the server is being shut down
void signal_logger(int signal)
{
  BOOST_LOG_TRIVIAL(fatal) << "Received signal " << signal << ", server is shutting down\n";
  exit(signal);
}

// Initialize main for logging
void log_init(const std::string log_directory, const std::string log_prefix)
{
  const int rotate_size = 10 * 1024 * 1024; // 10 MB
  const std::string log_format = "[%TimeStamp%][%ThreadID%][%Severity%]: %Message%";

  // Add file log parameters
  logging::add_file_log(
      keywords::file_name = log_directory + log_prefix + "%Y%m%d%H%M.log",
      keywords::rotation_size = rotate_size,
      keywords::time_based_rotation = logging::sinks::file::rotation_at_time_point(0, 0, 0), // Rotate at midnight
      keywords::format = log_format,
      keywords::auto_flush = true);

  // Add console log parameters
  logging::add_console_log(
      std::cout,
      keywords::format = log_format,
      keywords::auto_flush = true);

  // Filter out message with severity level lower than info
  logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);
}

void check_log_files(const boost::system::error_code& error, const std::string& log_directory, const std::string& log_prefix, int max_log_files, std::shared_ptr<boost::asio::steady_timer> timer)
{
  if (!error) {
    // Get a list of log files in the log directory
    std::vector<boost::filesystem::path> log_files;
    boost::filesystem::directory_iterator end_itr;
    for (boost::filesystem::directory_iterator itr(log_directory); itr != end_itr; ++itr) {
      if (boost::filesystem::is_regular_file(itr->status()) && itr->path().filename().string().find(log_prefix) == 0) {
        log_files.push_back(itr->path());
      }
    }

    // Sort the log files by creation time
    std::sort(log_files.begin(), log_files.end(),
        [](const boost::filesystem::path& a, const boost::filesystem::path& b) {
          return boost::filesystem::last_write_time(a) < boost::filesystem::last_write_time(b);
        });

    // Delete the oldest log file(s) if the maximum number of log files has been reached
    BOOST_LOG_TRIVIAL(info) << "Current amount of logs is: " << log_files.size();
    if (log_files.size() > max_log_files) {
      BOOST_LOG_TRIVIAL(info) << "Removing log: " << log_files.front();
      boost::filesystem::remove(log_files.front());
    }
    else
      BOOST_LOG_TRIVIAL(info) << "No logs need to be removed";
  }
  else
    BOOST_LOG_TRIVIAL(error) << "check logs error: " << error.message();

  timer->expires_after(std::chrono::minutes(1));
  timer->async_wait([timer, &log_directory, &log_prefix, max_log_files](const boost::system::error_code& error) {
    check_log_files(error, log_directory, log_prefix, max_log_files, timer);
  });
}

int main(int argc, char* argv[])
{
  // Initialize the logging
  const std::string log_directory = "log/";
  const std::string log_prefix = "server_log_";
  const int max_log_files = 10;

  signal(SIGINT, signal_logger);
  log_init(log_directory, log_prefix);
  logging::add_common_attributes();

  std::shared_ptr<boost::asio::io_service> io_service_ptr = std::make_shared<boost::asio::io_service>();

  std::shared_ptr<boost::asio::steady_timer> timer_ptr = std::make_shared<boost::asio::steady_timer>(*io_service_ptr);
  timer_ptr->expires_after(std::chrono::seconds(0));
  timer_ptr->async_wait([timer_ptr, &log_directory, &log_prefix, max_log_files](const boost::system::error_code& error) {
    check_log_files(error, log_directory, log_prefix, max_log_files, timer_ptr);
    });

  try
  {
    if (argc != 2)
    {
      // no config file was given/too many arguments
      BOOST_LOG_TRIVIAL(fatal) << "Usage: server <path_to_config>";
      return -1;
    }

    NginxConfig config;
    NginxConfigParser parser;

    BOOST_LOG_TRIVIAL(info) << "Starting config parsing";

    bool config_is_valid = parser.Parse(argv[1], &config);
    if (!config_is_valid)
    {
      BOOST_LOG_TRIVIAL(fatal) << "Invalid config file provided";
      return -1;
    }
    BOOST_LOG_TRIVIAL(info) << "Parsing Complete";


    int max_port = 65535;
    int port = config.get_listen_port();

    BOOST_LOG_TRIVIAL(info) << "Listening on port "
      << std::to_string(port);

    session s(*io_service_ptr);

    server serv(s, *io_service_ptr, config.get_listen_port());
    serv.init_server(config);

    serv.start_accept();

    BOOST_LOG_TRIVIAL(info) << "Now accepting connections";

    boost::asio::io_service::work work(*io_service_ptr);
    std::vector<std::thread> threads;
    unsigned int num_threads = std::thread::hardware_concurrency();
    for (unsigned int i = 0; i < num_threads; ++i) {
      threads.emplace_back([io_service_ptr]() {
        io_service_ptr->run();
      });
    }
    // serv.run();
    for (auto& thread : threads) {
      thread.join();
    }
  }
  catch (std::exception& e)
  {
    BOOST_LOG_TRIVIAL(error) << "Exception: " << e.what();
  }

  return 0;
}
