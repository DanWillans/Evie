#ifndef EVIE_LAYER_H_
#define EVIE_LAYER_H_

#include "evie/error.h"
#include "evie/events.h"

namespace evie {
class EVIE_API Layer
{
public:
  virtual ~Layer() = default;
  virtual void OnUpdate() = 0;
  virtual void OnEvent(Event& event) = 0;
  virtual void OnRender() = 0;
  virtual void Shutdown() {}
};
}// namespace evie


#endif// EVIE_LAYER_H_