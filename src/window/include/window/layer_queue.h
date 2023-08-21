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

  // using Iterator = std::deque<LayerWrapper>::iterator;
  // using ReverseIterator = std::deque<LayerWrapper>::reverse_iterator;

  // We could just use the `using` declaration above but I didn't want to expose the stl library interface.
  // I also wanted to write my own iterator to support ranges and static_assert concepts :)
  struct Iterator
  {
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = LayerWrapper;
    using pointer = value_type*;
    using reference = value_type&;
    using it_type = std::deque<value_type>::iterator;

    // Default constructor required to pass iterator static assert
    Iterator() { throw std::runtime_error{ "Not implemented" }; }
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

    Iterator& operator--()
    {
      --iter_;
      return *this;
    }

    Iterator operator--(int)// NOLINT
    {
      Iterator tmp = *this;
      --iter_;
      return tmp;
    }

    Iterator& operator-=(const difference_type& offset)
    {
      iter_ -= offset;
      return *this;
    }

    Iterator operator-(const difference_type& offset) const
    {
      Iterator tmp = *this;
      tmp -= offset;
      return tmp;
    }

    friend Iterator operator-(const difference_type offset, Iterator iter)
    {
      iter -= offset;
      return iter;
    }

    difference_type operator-(const Iterator& other) const { return iter_ - other.iter_; }

    Iterator& operator+=(const difference_type offset)
    {
      iter_ += offset;
      return *this;
    }

    Iterator operator+(const difference_type offset) const
    {
      Iterator tmp = *this;
      tmp += offset;
      return tmp;
    }

    friend Iterator operator+(const difference_type offset, Iterator iter)
    {
      iter += offset;
      return iter;
    }

    friend bool operator==(const Iterator& lhs, const Iterator& rhs) { return lhs.iter_ == rhs.iter_; }
    friend bool operator!=(const Iterator& lhs, const Iterator& rhs) { return !(lhs == rhs); }
    std::strong_ordering operator<=>(const Iterator& rhs) const
    {
      return iter_ <=> rhs.iter_;
    }

    reference& operator[](const difference_type offset) const { return iter_[offset]; }

  private:
    it_type iter_;
  };

  struct ReverseIterator{
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = LayerWrapper;
    using pointer = value_type*;
    using reference = value_type&;
    using it_type = std::deque<value_type>::reverse_iterator;

    // Default constructor required to pass iterator static assert
    ReverseIterator() { throw std::runtime_error{ "Not implemented" }; }
    explicit ReverseIterator(const it_type& iter) : iter_(iter) {}

    reference operator*() const { return *iter_; }
    pointer operator->() { return &(*iter_); }

    ReverseIterator& operator++()
    {
      ++iter_;
      return *this;
    }

    ReverseIterator operator++(int)// NOLINT
    {
      ReverseIterator tmp = *this;
      ++iter_;
      return tmp;
    }

    ReverseIterator& operator--()
    {
      --iter_;
      return *this;
    }

    ReverseIterator operator--(int)// NOLINT
    {
      ReverseIterator tmp = *this;
      --iter_;
      return tmp;
    }

    ReverseIterator& operator-=(const difference_type& offset)
    {
      iter_ -= offset;
      return *this;
    }

    ReverseIterator operator-(const difference_type& offset) const
    {
      ReverseIterator tmp = *this;
      tmp -= offset;
      return tmp;
    }

    friend ReverseIterator operator-(const difference_type offset, ReverseIterator iter)
    {
      iter -= offset;
      return iter;
    }

    difference_type operator-(const ReverseIterator& other) const { return iter_ - other.iter_; }

    ReverseIterator& operator+=(const difference_type offset)
    {
      iter_ += offset;
      return *this;
    }

    ReverseIterator operator+(const difference_type offset) const
    {
      ReverseIterator tmp = *this;
      tmp += offset;
      return tmp;
    }

    friend ReverseIterator operator+(const difference_type offset, ReverseIterator iter)
    {
      iter += offset;
      return iter;
    }

    friend bool operator==(const ReverseIterator& lhs, const ReverseIterator& rhs) { return lhs.iter_ == rhs.iter_; }
    friend bool operator!=(const ReverseIterator& lhs, const ReverseIterator& rhs) { return !(lhs == rhs); }
    std::strong_ordering operator<=>(const ReverseIterator& rhs) const
    {
      return iter_ <=> rhs.iter_;
    }

    reference& operator[](const difference_type offset) const { return iter_[offset]; }

    private:
    it_type iter_;
  };

  [[nodiscard]] Iterator EVIE_API begin() { return Iterator(layers_.begin()); }
  [[nodiscard]] Iterator EVIE_API end() { return Iterator(layers_.end()); }
  [[nodiscard]] ReverseIterator EVIE_API rbegin() { return ReverseIterator(layers_.rbegin()); }
  [[nodiscard]] ReverseIterator EVIE_API rend() { return ReverseIterator(layers_.rend()); }


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
