#include <memory>

#include "evie/logging.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace evie {
std::shared_ptr<spdlog::logger> LoggingManager::engine_logger_ = spdlog::stdout_color_mt("EVIE");
std::shared_ptr<spdlog::logger> LoggingManager::client_logger_ = spdlog::stdout_color_mt("CLIENT");

spdlog::logger* LoggingManager::GetClientLogger(){
  return client_logger_.get();
}

spdlog::logger* LoggingManager::GetEngineLogger(){
  return engine_logger_.get();
}

void LoggingManager::Init()
{
  // Engine logger setup
  engine_logger_->set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
  engine_logger_->set_level(spdlog::level::trace);

  // Client logger setup
  client_logger_->set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
  client_logger_->set_level(spdlog::level::trace);
}

}// namespace evie