#ifndef EVIE_APPLICATION_H_
#define EVIE_APPLICATION_H_

#include <memory>

#include "evie/core.h"
#include "evie/error.h"
#include "evie/input_manager.h"
#include "evie/layer.h"
#include "evie/types.h"
#include "evie/window.h"

namespace evie {
class EVIE_API Application
{
public:
  Application();
  Application(const Application&) = delete;
  Application(Application&& app) = delete;
  Application& operator=(const Application& app) = delete;
  Application& operator=(Application&& app) = delete;
  virtual ~Application();

  void Run();
  [[nodiscard]] Error Initialise(const WindowProperties& props);
  [[nodiscard]] const IInputManager* GetInputManager() const;
  [[nodiscard]] IWindow* GetWindow() const;
  void PushLayerFront(Layer& layer);
  void PushLayerBack(Layer& layer);
  void Shutdown();

private:
  // Use impl so that we don't have to export types and implementation to the user.
  // For example there is no need to export the LayerQueue header file as the Application layer API should expose to the
  // game developer what they can do.
  // Let's revisit this later because I'm not the biggest fan of Impl - It bloats the code.
  class Impl;
  Impl* impl_;

  // These are fine to expose
  void CloseWindow() { running_ = false; }
  bool running_{ true };
  bool initialised_{ false };
};
}// namespace evie

#endif//  EVIE_APPLICATION_H_