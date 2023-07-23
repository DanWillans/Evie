#include <doctest/doctest.h>

#include "evie/ids.h"
#include "evie/layer.h"

#include "window/layer_stack.h"

namespace {
class TestLayer final : public evie::Layer
{
public:
  void OnUpdate() override {}
  ~TestLayer() {}
};
}// namespace

TEST_CASE("LayerStack Tests")
{
  evie::LayerStack layer_stack;
  TestLayer layer;
  TestLayer layer1;
  TestLayer layer2;
  TestLayer layer3;
  SUBCASE("Add layers and check returned LayerID increments")
  {
    evie::LayerID id_1 = layer_stack.PushLayer(layer);
    evie::LayerID id_2 = layer_stack.PushLayer(layer1);
    evie::LayerID id_3 = layer_stack.PushLayer(layer2);
    evie::LayerID id_4 = layer_stack.PushLayer(layer3);
    REQUIRE(id_1.Get() == 0);
    REQUIRE(id_2.Get() == 1);
    REQUIRE(id_3.Get() == 2);
    REQUIRE(id_4.Get() == 3);
    SUBCASE("Check that the LayerStack iterator works")
    {
      REQUIRE(std::distance(layer_stack.begin(), layer_stack.end()) == 4);
    }
    SUBCASE("Check that removing a layer works")
    {
      evie::Error err = layer_stack.RemoveLayer(id_3);
      REQUIRE(err.Good());
      SUBCASE("Check that removing a non existent layer returns an error")
      {
        err = layer_stack.RemoveLayer(id_3);
        REQUIRE(err.Bad());
      }
      SUBCASE("Confirm the layer doesn't show up when iterating")
      {
        REQUIRE(std::distance(layer_stack.begin(), layer_stack.end()) == 3);
      }
    }
  }
}