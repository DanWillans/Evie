#include <doctest/doctest.h>

#include <vector>

#include "evie/ids.h"
#include "evie/ecs/component_array.hpp"
#include "evie/ecs/component_manager.hpp"

namespace {
struct TestComponent1
{
  int a;
};
struct TestComponent2
{
  int a;
};
struct TestComponent3
{
  int a;
};
struct TestComponent4
{
  int a;
};
}// namespace

using namespace evie;

TEST_CASE("Test ComponentArray")
{
  ComponentArray<TestComponent1> comp_array(ComponentID<TestComponent1>(0));
  EntityID id_0{ 0 };
  EntityID id_1{ 1 };
  EntityID id_2{ 2 };
  EntityID id_3{ 3 };
  EntityID id_4{ 4 };
  comp_array.AddComponent(id_0, { 0 });
  comp_array.AddComponent(id_1, { 1 });
  comp_array.AddComponent(id_2, { 2 });
  comp_array.AddComponent(id_3, { 3 });
  comp_array.AddComponent(id_4, { 4 });
  auto ValidCheckAndNext = [&](EntityID id, int val) {
    REQUIRE(comp_array.GetComponent(id).a ==  val);
  };
  REQUIRE_EQ(comp_array.Size(), 5);
  // Check all components exist
  ValidCheckAndNext(id_0, 0);
  ValidCheckAndNext(id_1, 1);
  ValidCheckAndNext(id_2, 2);
  ValidCheckAndNext(id_3, 3);
  ValidCheckAndNext(id_4, 4);
  // Remove components
  comp_array.RemoveComponent(id_1);
  comp_array.RemoveComponent(id_2);
  REQUIRE_EQ(comp_array.Size(), 3);
  // Check left over components exist
  ValidCheckAndNext(id_0, 0.0);
  ValidCheckAndNext(id_4, 4.0);
  ValidCheckAndNext(id_3, 3.0);
  REQUIRE_EQ(comp_array.Size(), 3);
  // Add components back in a different order
  comp_array.AddComponent(id_2, { 2 });
  REQUIRE_EQ(comp_array.Size(), 4);
  comp_array.AddComponent(id_1, { 1 });
  REQUIRE_EQ(comp_array.Size(), 5);
  // Now get a new iterator and check that the order is 0,2,1,3,4
  // This order is because we removed 1, 2 and then put back 2, 1 and the free slots
  // would have been used up first before allocating new ones
  ValidCheckAndNext(id_0, 0.0);
  ValidCheckAndNext(id_4, 4.0);
  ValidCheckAndNext(id_3, 3.0);
  ValidCheckAndNext(id_2, 2.0);
  ValidCheckAndNext(id_1, 1.0);
  EntityID id_5{ 5 };
  comp_array.AddComponent(id_5, { 5 });
  REQUIRE_EQ(comp_array.Size(), 6);
  ValidCheckAndNext(id_0, 0.0);
  ValidCheckAndNext(id_4, 4.0);
  ValidCheckAndNext(id_3, 3.0);
  ValidCheckAndNext(id_2, 2.0);
  ValidCheckAndNext(id_1, 1.0);
  ValidCheckAndNext(id_5, 5.0);
}

TEST_CASE("Test ComponentManager")
{
  ComponentManager comp_manager;
  EntityID id_0{ 0 };
  EntityID id_1{ 1 };
  EntityID id_2{ 2 };
  EntityID id_3{ 3 };
  EntityID id_4{ 4 };
  // Register all our components first
  auto comp_id_1 = comp_manager.RegisterComponent<TestComponent1>();
  auto comp_id_2 = comp_manager.RegisterComponent<TestComponent2>();
  auto comp_id_3 = comp_manager.RegisterComponent<TestComponent3>();
  auto comp_id_4 = comp_manager.RegisterComponent<TestComponent4>();
  // Register components to entities
  auto RegEntityAndCheck = [&]<typename T>(EntityID id, ComponentID<T> comp_id, T comp) {
    auto err = comp_manager.AddComponent(id, comp_id, comp);
    REQUIRE(err.Good());
  };
  // Register entity_0 components
  RegEntityAndCheck(id_0, comp_id_1, { 1 });
  RegEntityAndCheck(id_0, comp_id_2, { 2 });
  RegEntityAndCheck(id_0, comp_id_3, { 3 });
  RegEntityAndCheck(id_0, comp_id_4, { 4 });
  // Register entity 1 components
  RegEntityAndCheck(id_1, comp_id_1, { 5 });
  RegEntityAndCheck(id_1, comp_id_2, { 6 });
  // Register entity 2 components
  RegEntityAndCheck(id_2, comp_id_3, { 7 });
  RegEntityAndCheck(id_2, comp_id_4, { 8 });
  // Register entity 3 components
  RegEntityAndCheck(id_3, comp_id_1, { 9 });
  RegEntityAndCheck(id_3, comp_id_4, { 10 });
  // Register entity 4 components
  RegEntityAndCheck(id_4, comp_id_2, { 11 });
  RegEntityAndCheck(id_4, comp_id_3, { 12 });

  auto ValidCheckAndNext = [&]<typename T>(EntityID entity_id, T& comp_id, int val) {
    REQUIRE(comp_manager.GetComponent(entity_id, comp_id).a == val);
  };
  // Check all the components that we registered exist
  ValidCheckAndNext(id_0, comp_id_1, 1);
  ValidCheckAndNext(id_1, comp_id_1, 5);
  ValidCheckAndNext(id_3, comp_id_1, 9);
  REQUIRE(comp_manager.GetComponentCount(comp_id_1) == 3);

  ValidCheckAndNext(id_0, comp_id_2, 2);
  ValidCheckAndNext(id_1, comp_id_2, 6);
  ValidCheckAndNext(id_4, comp_id_2, 11);
  REQUIRE(comp_manager.GetComponentCount(comp_id_2) == 3);

  ValidCheckAndNext(id_0, comp_id_3, 3);
  ValidCheckAndNext(id_2, comp_id_3, 7);
  ValidCheckAndNext(id_4, comp_id_3, 12);
  REQUIRE(comp_manager.GetComponentCount(comp_id_3) == 3);

  ValidCheckAndNext(id_0, comp_id_4, 4);
  ValidCheckAndNext(id_2, comp_id_4, 8);
  ValidCheckAndNext(id_3, comp_id_4, 10);
  REQUIRE(comp_manager.GetComponentCount(comp_id_4) == 3);

  // Now inform the component manager that the entity 0 has been destroyed.
  // All of it's component entries should have disappeared.
  comp_manager.EntityDestroyed(id_0);

  ValidCheckAndNext(id_3, comp_id_1, 9);
  ValidCheckAndNext(id_1, comp_id_1, 5);
  REQUIRE(comp_manager.GetComponentCount(comp_id_1) == 2);

  ValidCheckAndNext(id_4, comp_id_2, 11);
  ValidCheckAndNext(id_1, comp_id_2, 6);
  REQUIRE(comp_manager.GetComponentCount(comp_id_2) == 2);

  ValidCheckAndNext(id_4, comp_id_3, 12);
  ValidCheckAndNext(id_2, comp_id_3, 7);
  REQUIRE(comp_manager.GetComponentCount(comp_id_3) == 2);

  ValidCheckAndNext(id_3, comp_id_4, 10);
  ValidCheckAndNext(id_2, comp_id_4, 8);
  REQUIRE(comp_manager.GetComponentCount(comp_id_4) == 2);

  // Now remove just entity 1 TestComponent2
  comp_manager.RemoveComponent(id_1, comp_id_2);

  ValidCheckAndNext(id_4, comp_id_2, 11);
  REQUIRE(comp_manager.GetComponentCount(comp_id_2) == 1);

  // And the same with entity 4
  comp_manager.RemoveComponent(id_4, comp_id_2);
  REQUIRE(comp_manager.GetComponentCount(comp_id_2) == 0);
}