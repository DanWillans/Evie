#include <algorithm>

#include "window/layer_stack.h"

#include "evie/ids.h"
#include "evie/layer.h"

namespace evie {
LayerID LayerStack::PushLayer(Layer& layer)
{
  LayerID identifier(layer_count_++);
  layers_.push_back(LayerWrapper{ identifier, &layer });
  return identifier;
}

Error LayerStack::RemoveLayer(LayerID layer_id)
{
  evie::Error err = Error::OK();
  auto layer_it = std::find_if(
    layers_.begin(), layers_.end(), [&layer_id](const LayerWrapper& wrapper) { return layer_id == wrapper.id; });

  if (layer_it == layers_.end()) {
    return evie::Error("Can't remove layer, no layer found with ID provided");
  }

  layers_.erase(layer_it);
  return err;
}


}// namespace evie