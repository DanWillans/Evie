#ifndef EVIE_LAYER_H_
#define EVIE_LAYER_H_

namespace evie {
class Layer
{
public:
  virtual ~Layer() = default;
  virtual void OnUpdate() = 0;
};
}// namespace evie


#endif// EVIE_LAYER_H_