#include <memory>
#include <string_view>

#include "evie/logging.h"
#include "loggers.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"


namespace evie {
// No lint these static variables for now until we have a better
// way to deal with loggers.
std::unique_ptr<Logger> ClientLoggerManager::logger_ = nullptr;// NOLINT
std::unique_ptr<Logger> EngineLoggerManager::logger_ = nullptr;// NOLINT

Logger::Logger(Logger&& other) noexcept
  : console_sink_(std::move(other.console_sink_)), logger_(std::make_unique<spdlog::logger>(*other.logger_))
{}

Logger::Logger(const std::string& name) : console_sink_(std::make_shared<spdlog::sinks::stdout_color_sink_mt>())
{
  // Setup console sink
  console_sink_->set_level(spdlog::level::info);
  console_sink_->set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

  // Setup logger and bind console to it
  logger_ = std::make_unique<spdlog::logger>(spdlog::logger(name, { console_sink_ }));
}

Logger* ClientLoggerManager::GetLogger()
{
  Logger logger("ClientLogger");
  if (!logger_) { logger_ = std::make_unique<Logger>(std::move(logger)); }
  return logger_.get();
}

Logger* EngineLoggerManager::GetLogger()
{
  Logger logger("EngineLogger");
  if (!logger_) { logger_ = std::make_unique<Logger>(std::move(logger)); }
  return logger_.get();
}

}// namespace evie