#ifndef EVIE_LAYER_STACK_H_
#define EVIE_LAYER_STACK_H_

#include <deque>
#include <iterator>
#include <ranges>
#include <stdexcept>

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

/**
 * @brief
 * Users can push layers to the front or back of the LayerQueue
 * This class will store a pointer to layers and not take ownership.
 */
class LayerQueue
{
public:
  LayerID EVIE_API PushFront(Layer& layer);
  LayerID EVIE_API PushBack(Layer& layer);
  Error EVIE_API RemoveLayer(LayerID layer_id);

  using Iterator = std::deque<LayerWrapper>::iterator;
  using ReverseIterator = std::deque<LayerWrapper>::reverse_iterator;

  [[nodiscard]] Iterator EVIE_API begin() { return layers_.begin(); }
  [[nodiscard]] Iterator EVIE_API end() { return layers_.end(); }
  [[nodiscard]] ReverseIterator EVIE_API rbegin() { return layers_.rbegin(); }
  [[nodiscard]] ReverseIterator EVIE_API rend() { return layers_.rend(); }


private:
  int layer_count_{ 0 };
  std::deque<LayerWrapper> layers_{};
};

// Check the container satisfies the C++20 concepts of ranges and iterators
static_assert(std::ranges::range<LayerQueue>);
static_assert(std::random_access_iterator<LayerQueue::Iterator>);
static_assert(std::random_access_iterator<LayerQueue::ReverseIterator>);

}// namespace evie


#endif// !EVIE_LAYER_STACK_H_
