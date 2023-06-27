#include "evie/application.h"
#include "evie/logging.h"
#include "events.h"


namespace evie {
void Application::Run()
{
  LoggingManager::Init();
  EV_INFO("Logging Initialised");
  while (true) {}
}
}// namespace evie
