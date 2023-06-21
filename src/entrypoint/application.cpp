#include "evie/application.h"
#include "evie/logging.h"
#include <iostream>

namespace evie {
void Application::Run()
{
  LoggingManager::Init();
  const char* name = "Dan";
  APP_INFO("Hello {}", name);
  while (true) { std::cout << placeholder_++ << "\n"; }
}
}// namespace evie
