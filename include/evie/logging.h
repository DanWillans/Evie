#ifndef EVIE_LOGGING_H_
#define EVIE_LOGGING_H_

#include <memory>
#include <string>

namespace evie {

class Logger;

// Static class that encapsulates the client logger
class ClientLoggerManager
{
public:
  static Logger* GetLogger();

private:
  static std::unique_ptr<Logger> logger_;
};
}// namespace evie

// #define APP_INFO(...) evie::ClientLoggerManager::GetLogger->Info(__VA_ARGS__)// NOLINT
// #define APP_WARN(...) evie::ClientLoggerManager::GetLogger->Warn(__VA_ARGS__)// NOLINT
// #define APP_TRACE(...) evie::ClientLoggerManager::GetLogger->Trace(__VA_ARGS__)// NOLINT
// #define APP_ERROR(...) evie::ClientLoggerManager::GetLogger->Error(__VA_ARGS__)// NOLINT
// #define APP_DEBUG(...) evie::ClientLoggerManager::GetLogger->Debug(__VA_ARGS__)// NOLINT

#endif// EVIE_LOGGING_H_