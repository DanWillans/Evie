#ifndef EVIE_ENTRYPOINT_H_
#define EVIE_ENTRYPOINT_H_

#include <iostream>
#include <memory>

#include "evie/application.h"

#ifdef EVIE_PLATFORM_WINDOWS

extern std::unique_ptr<evie::Application> CreateApplication();

int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv)
{
  auto app = CreateApplication();
  if (app) {
    app->Run();
  }
  app->Shutdown();
  return 0;
}

#else

extern std::unique_ptr<evie::Application> CreateApplication();

int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv)
{
  auto app = CreateApplication();
  if (app) {
    app->Run();
  }
  app->Shutdown();
  return 0;
}

#endif

#endif//  EVIE_ENTRYPOINT_H_
