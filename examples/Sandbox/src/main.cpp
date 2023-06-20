#include <memory>

#include "evie/evie.h"
#include "evie/logging.h"

class Sandbox : public evie::Application
{
public:
private:
};

std::unique_ptr<evie::Application> CreateApplication() { 
  // const char* name = "Dan";
  // APP_INFO("Hello! My name is {}", name);
  auto logger = evie::ClientLoggerManager::GetLogger();
  logger->Info("Hello");
  return std::make_unique<Sandbox>(); }
