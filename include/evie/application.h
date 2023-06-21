#ifndef EVIE_APPLICATION_H_
#define EVIE_APPLICATION_H_

#include "evie/core.h"

namespace evie{
  class EVIE_API Application {
    public:
      Application() = default;
      Application(const Application&) = default;
      Application(Application&& app) = default;
      Application &operator=(const Application& app) = default;
      Application &operator=(Application&& app) = default;
      virtual ~Application() = default;

      void Run();
    private:
      int placeholder_ {0};
  };
}

#endif //  EVIE_APPLICATION_H_