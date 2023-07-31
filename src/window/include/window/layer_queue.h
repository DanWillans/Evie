#ifndef EVIE_LAYER_STACK_H_
#define EVIE_LAYER_STACK_H_

#include <deque>
#include <iterator>

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

  // using Iterator = std::deque<LayerWrapper>::iterator;

  // We could just use the `using` declaration above but I didn't want to expose the stl library interface.
  // I also wanted to write my own iterator :)
  struct Iterator
  {
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = LayerWrapper;
    using pointer = value_type*;
    using reference = value_type&;
    using it_type = std::deque<value_type>::iterator;

    explicit Iterator(const it_type& iter) : iter_(iter) {}

    reference operator*() const { return *iter_; }
    pointer operator->() { return &(*iter_); }

    Iterator& operator++()
    {
      ++iter_;
      return *this;
    }

    Iterator operator++(int)// NOLINT
    {
      Iterator tmp = *this;
      ++iter_;
      return tmp;
    }

    friend bool operator==(const Iterator& lhs, const Iterator& rhs) { return lhs.iter_ == rhs.iter_; }
    friend bool operator!=(const Iterator& lhs, const Iterator& rhs) { return !(lhs == rhs); }

  private:
    it_type iter_;
  };


  [[nodiscard]] Iterator EVIE_API begin() { return Iterator(layers_.begin()); }
  [[nodiscard]] Iterator EVIE_API end() { return Iterator(layers_.end()); }

private:
  int layer_count_{ 0 };
  std::deque<LayerWrapper> layers_{};
};
}// namespace evie


#endif// !EVIE_LAYER_STACK_H_
