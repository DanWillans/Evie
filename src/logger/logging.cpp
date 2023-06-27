#include <memory>
#include <string_view>

#include "evie/logging.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>


namespace evie {
void LoggingManager::Init()
{
  // Engine logger setup
  auto* engine_logger_ = GetInternalEngineLogger();
  engine_logger_->set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
  engine_logger_->set_level(spdlog::level::trace);

  // Client logger setup
  auto* client_logger_ = GetInternalClientLogger();
  client_logger_->set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
  client_logger_->set_level(spdlog::level::trace);
}

}// namespace evie