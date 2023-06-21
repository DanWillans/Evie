#ifndef EVIE_ENTRYPOINT_H_
#define EVIE_ENTRYPOINT_H_

#include <memory>
#include <iostream>

#include "evie/application.h"

#ifdef EVIE_PLATFORM_WINDOWS

extern std::unique_ptr<evie::Application> CreateApplication();

int main([[maybe_unused]]int argc, [[maybe_unused]]const char** argv) {
    auto app = CreateApplication();
    app->Run();
    return 0;
}

#endif

#endif //  EVIE_ENTRYPOINT_H_
