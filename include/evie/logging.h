#ifndef EVIE_LOGGING_H_
#define EVIE_LOGGING_H_

#include "evie/core.h"

#include <iostream>
#include <memory>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace evie {

class LoggingManager
{
public:
  static EVIE_API void Init();
  static EVIE_API spdlog::logger* GetClientLogger();
  static EVIE_API spdlog::logger* GetEngineLogger();

private:
  static std::shared_ptr<spdlog::logger> engine_logger_;
  static std::shared_ptr<spdlog::logger> client_logger_;
};
}// namespace evie

#define APP_INFO(...) evie::LoggingManager::GetClientLogger()->info(__VA_ARGS__)// NOLINT
#define APP_WARN(...) evie::LoggingManager::GetClientLogger()->warn(__VA_ARGS__)// NOLINT
#define APP_TRACE(...) evie::LoggingManager::GetClientLogger()->trace(__VA_ARGS__)// NOLINT
#define APP_ERROR(...) evie::LoggingManager::GetClientLogger()->error(__VA_ARGS__)// NOLINT
#define APP_DEBUG(...) evie::LoggingManager::GetClientLogger()->debug(__VA_ARGS__)// NOLINT

#define EV_INFO(...) evie::LoggingManager::GetEngineLogger()->info(__VA_ARGS__)// NOLINT
#define EV_WARN(...) evie::LoggingManager::GetEngineLogger()->warn(__VA_ARGS__)// NOLINT
#define EV_TRACE(...) evie::LoggingManager::GetEnginelogger()->trace(__VA_ARGS__)// NOLINT
#define EV_ERROR(...) evie::LoggingManager::GetEnginelogger()->error(__VA_ARGS__)// NOLINT
#define EV_DEBUG(...) evie::LoggingManager::GetEnginelogger()->debug(__VA_ARGS__)// NOLINT

#endif// EVIE_LOGGING_H_