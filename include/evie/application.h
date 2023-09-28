#ifndef EVIE_APPLICATION_H_
#define EVIE_APPLICATION_H_

#include <memory>

#include "evie/core.h"
#include "evie/error.h"
#include "evie/input_manager.h"
#include "evie/layer.h"
#include "evie/types.h"

namespace evie {
class EVIE_API Application
{
public:
  EVIE_API Application();
  Application(const Application&) = delete;
  Application(Application&& app) = delete;
  Application& operator=(const Application& app) = delete;
  Application& operator=(Application&& app) = delete;
  virtual EVIE_API ~Application();

  void EVIE_API Run();
  [[nodiscard]] Error EVIE_API Initialise(const WindowProperties& props);
  [[nodiscard]] const EVIE_API IInputManager* GetInputManager() const;
  void EVIE_API PushLayerFront(Layer& layer);
  void EVIE_API PushLayerBack(Layer& layer);
  void EVIE_API Shutdown();

private:
  // Use impl so that we don't have to export types and implementation to the user.
  // For example there is no need to export the LayerQueue header file as the Application layer API should expose to the
  // game developer what they can do.
  // Alternatively, we could make everything an interface and export the interfaces like IInputManager.
  // Let's revisit this later because I'm not the biggest fan of Impl - It bloats the code.
  class Impl;
  Impl* impl_;

  std::unique_ptr<IInputManager> input_manager_;

  // These are fine to expose
  void CloseWindow() { running_ = false; }
  bool running_{ true };
  bool initialised_{ false };
};
}// namespace evie

#endif//  EVIE_APPLICATION_H_