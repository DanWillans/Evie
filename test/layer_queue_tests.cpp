#include <algorithm>
#include <doctest/doctest.h>

#include <vector>

#include "evie/ids.h"
#include "evie/layer.h"

#include "window/layer_queue.h"

namespace {

class TestLayer final : public evie::Layer
{
public:
  void OnUpdate() override {}
  ~TestLayer() {}
};

}// namespace

TEST_CASE("LayerQueue Tests")
{
  evie::LayerQueue layer_queue;
  TestLayer layer;
  TestLayer layer1;
  TestLayer layer2;
  TestLayer layer3;
  TestLayer layer4;
  SUBCASE("PushBack() layers and check returned LayerID increments")
  {
    evie::LayerID id_1 = layer_queue.PushBack(layer);
    evie::LayerID id_2 = layer_queue.PushBack(layer1);
    evie::LayerID id_3 = layer_queue.PushBack(layer2);
    evie::LayerID id_4 = layer_queue.PushBack(layer3);
    REQUIRE(id_1.Get() == 0);
    REQUIRE(id_2.Get() == 1);
    REQUIRE(id_3.Get() == 2);
    REQUIRE(id_4.Get() == 3);
    SUBCASE("Check that the LayerQueue iterator works")
    {
      REQUIRE(std::distance(layer_queue.begin(), layer_queue.end()) == 4);
      int count{ 0 };
      for (const auto& lay : layer_queue) {
        REQUIRE(lay.id.Get() == count++);
      }
    }
    SUBCASE("PushFront() layers and check returned LayerID increments")
    {
      evie::LayerID id_5 = layer_queue.PushFront(layer4);
      REQUIRE(id_5.Get() == 4);
      REQUIRE(std::distance(layer_queue.begin(), layer_queue.end()) == 5);
      SUBCASE("Check order of IDs in layer_queue")
      {
        std::vector<int> id_vec;
        std::transform(layer_queue.begin(), layer_queue.end(), std::back_inserter(id_vec), [](evie::LayerWrapper& l) {
          return l.id.Get();
        });
        REQUIRE(id_vec == std::vector<int>{ 4, 0, 1, 2, 3 });
      }
    }
    SUBCASE("Check that removing a layer works")
    {
      evie::Error err = layer_queue.RemoveLayer(id_3);
      REQUIRE(err.Good());
      SUBCASE("Check that removing a non existent layer returns an error")
      {
        err = layer_queue.RemoveLayer(id_3);
        REQUIRE(err.Bad());
      }
      SUBCASE("Confirm the layer doesn't show up when iterating")
      {
        REQUIRE(std::distance(layer_queue.begin(), layer_queue.end()) == 3);
        std::vector<int> id_vec;
        std::transform(layer_queue.begin(), layer_queue.end(), std::back_inserter(id_vec), [](evie::LayerWrapper& l) {
          return l.id.Get();
        });
        REQUIRE(id_vec == std::vector<int>{ 0, 1, 3 });
      }
    }
  }
}