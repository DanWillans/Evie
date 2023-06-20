#ifndef EVIE_LOGGER_H_
#define EVIE_LOGGER_H_

#include <memory>
#include <string_view>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>


namespace evie {
class LoggerManager;

class Logger
{
public:
  explicit Logger(const std::string& name);
  // Just allow move constructor for now and delete every other constructor
  Logger(Logger&& other) noexcept;
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
  Logger& operator=(Logger&&) = delete;
  ~Logger() = default;

  template<typename... Args> void Info(Args&&... args) { logger_->info(args...); }
  template<typename... Args> void Warn(Args&&... args) { logger_->warn(args...); }
  template<typename... Args> void Trace(Args&&... args) { logger_->trace(args...); }
  template<typename... Args> void Error(Args&&... args) { logger_->error(args...); }
  template<typename... Args> void Debug(Args&&... args) { logger_->debug(args...); }

private:
  std::unique_ptr<spdlog::logger> logger_ = nullptr;
  std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink_ = nullptr;
};

}// namespace evie

#endif//  EVIE_LOGGER_H_