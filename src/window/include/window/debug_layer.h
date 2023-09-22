#ifndef EVIE_DEBUG_LAYER_H_
#define EVIE_DEBUG_LAYER_H_

#include "evie/layer.h"
#include "imgui.h"

#include "GLFW/glfw3.h"

namespace evie {

class EVIE_API DebugLayer final : public Layer
{
  public:
  explicit DebugLayer(GLFWwindow* window);
  ~DebugLayer() override;
  void OnUpdate() override;
  void OnEvent(Event& event) override;
};

}// namespace evie

#endif// !EVIE_DEBUG_LAYER_H_