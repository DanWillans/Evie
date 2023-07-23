#ifndef EVIE_LAYER_STACK_H_
#define EVIE_LAYER_STACK_H_

#include <iterator>
#include <vector>

#include "evie/core.h"
#include "evie/error.h"
#include "evie/ids.h"
#include "evie/layer.h"

namespace evie {

struct LayerWrapper
{
  LayerID id;
  Layer* layer;
};

/** A class that handles the layers in the engine.
  Users can push and pop layers and overlays.
  Overlays are still layers but sit ontop of layers in the layerstack, therefore they receive events before other
  layers. This class will store a pointer to layers and not take ownership.
 */
class LayerStack
{
public:
  LayerID EVIE_API PushLayer(Layer& layer);
  Error EVIE_API RemoveLayer(LayerID layer_id);

  // A custom iterator to return the Layer from the LayerStack vector rather than a LayerWrapper
  struct Iterator
  {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = Layer;
    using pointer = value_type*;
    using reference = value_type&;
    explicit Iterator(LayerWrapper* ptr) : ptr_(ptr) {}

    reference operator*() const { return *ptr_->layer; }
    pointer operator->() { return ptr_->layer; }

    Iterator& operator++()
    {
      ptr_++;// NOLINT
      return *this;
    }

    Iterator operator++(int)// NOLINT
    {
      Iterator tmp = *this;
      ptr_++;
      return tmp;
    }

    friend bool operator==(const Iterator& a, const Iterator& b) { return a.ptr_ == b.ptr_; }
    friend bool operator!=(const Iterator& a, const Iterator& b) { return a.ptr_ != b.ptr_; }

  private:
    LayerWrapper* ptr_;
  };

  [[nodiscard]] Iterator EVIE_API begin() { return Iterator(&(*layers_.begin())); }
  [[nodiscard]] Iterator EVIE_API end() { return Iterator(&(*layers_.end())); }

private:
  int layer_count_{ 0 };
  std::vector<LayerWrapper> layers_;
};
}// namespace evie


#endif// !EVIE_LAYER_STACK_H_
