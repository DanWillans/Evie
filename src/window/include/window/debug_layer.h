#ifndef EVIE_DEBUG_LAYER_H_
#define EVIE_DEBUG_LAYER_H_

#include "evie/error.h"
#include "evie/layer.h"

namespace evie {

class EVIE_API DebugLayer final : public Layer
{
public:
  // void* window is a pointer to the native GLFW window.
  // If we don't like this not being explicit without looking at the declaration then let's just change it to
  // GLFWwindow* and get the caller to cast it.
  explicit DebugLayer(void* window);
  ~DebugLayer() override = default;
  void OnUpdate() override;
  void OnRender() override;
  void OnEvent(Event& event) override;
  void Shutdown() override;
};

}// namespace evie

#endif// !EVIE_DEBUG_LAYER_H_