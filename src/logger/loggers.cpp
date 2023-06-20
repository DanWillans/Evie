#include <memory>
#include <string_view>

#include "evie/logging.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>


namespace evie {
// No lint these static variables for now until we have a better
// way to deal with loggers.
std::shared_ptr<spdlog::logger> LoggingManager::engine_logger_ = nullptr;// NOLINT
std::shared_ptr<spdlog::logger> LoggingManager::client_logger_ = nullptr;// NOLINT

void LoggingManager::Init(){
  // Engine setup logger
  engine_logger_ = spdlog::stdout_color_mt("EVIE");
  engine_logger_->set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
  engine_logger_->set_level(spdlog::level::trace);

  // Client setup logger
  client_logger_ = spdlog::stdout_color_mt("CLIENT");
  client_logger_->set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
  client_logger_->set_level(spdlog::level::trace);
}

}// namespace evie