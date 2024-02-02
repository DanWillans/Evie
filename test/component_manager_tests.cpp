#include "evie/ids.h"
#include <doctest/doctest.h>

#include <vector>

#include <evie/ecs/component_array.hpp>
#include <evie/ecs/component_manager.hpp>

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
struct TestComponent5
{
  int a;
};
std::vector<evie::ComponentWrapper<TestComponent1>> CreateComponentWrapperVec(int amount, bool dirty = false)
{
  std::vector<evie::ComponentWrapper<TestComponent1>> return_vec;
  for (int i = 0; i < amount; ++i) {
    return_vec.push_back({ { i }, evie::EntityID(static_cast<unsigned long>(i)) });
    return_vec.back().dirty = dirty;
  }
  return return_vec;
}
}// namespace

TEST_CASE("Test ComponentIterator with various dirty entities")
{
  auto transform_comp_vec = CreateComponentWrapperVec(16);
  transform_comp_vec[1].dirty = true;
  transform_comp_vec[4].dirty = true;
  transform_comp_vec[5].dirty = true;
  transform_comp_vec[6].dirty = true;
  transform_comp_vec[7].dirty = true;
  transform_comp_vec[8].dirty = true;
  transform_comp_vec[9].dirty = true;
  transform_comp_vec[10].dirty = true;
  transform_comp_vec[12].dirty = true;
  transform_comp_vec[14].dirty = true;
  transform_comp_vec[15].dirty = true;
  evie::ComponentIterator<TestComponent1> comp_it(transform_comp_vec);
  auto ValidCheckAndNext = [&](float val) {
    REQUIRE(comp_it.Valid());
    REQUIRE(comp_it->a == val);
    comp_it.Next();
  };
  ValidCheckAndNext(0.0);
  ValidCheckAndNext(2.0);
  ValidCheckAndNext(3.0);
  ValidCheckAndNext(11.0);
  ValidCheckAndNext(13.0);
  REQUIRE(!comp_it.Valid());
}

TEST_CASE("Test ComponentIterator with all dirty entities")
{
  auto transform_comp_vec = CreateComponentWrapperVec(16, true);
  evie::ComponentIterator<TestComponent1> comp_it(transform_comp_vec);
  // Should instantly be not valid as all entites are dirty.
  REQUIRE(!comp_it.Valid());
  comp_it.Next();
  REQUIRE(!comp_it.Valid());
}

TEST_CASE("Test ComponentIterator with all non dirty entiries")
{
  auto transform_comp_vec = CreateComponentWrapperVec(16);
  evie::ComponentIterator<TestComponent1> comp_it(transform_comp_vec);
  auto ValidCheckAndNext = [&](float val) {
    REQUIRE(comp_it.Valid());
    REQUIRE(comp_it->a == val);
    comp_it.Next();
  };
  ValidCheckAndNext(0.0);
  ValidCheckAndNext(1.0);
  ValidCheckAndNext(2.0);
  ValidCheckAndNext(3.0);
  ValidCheckAndNext(4.0);
  ValidCheckAndNext(5.0);
  ValidCheckAndNext(6.0);
  ValidCheckAndNext(7.0);
  ValidCheckAndNext(8.0);
  ValidCheckAndNext(9.0);
  ValidCheckAndNext(10.0);
  ValidCheckAndNext(11.0);
  ValidCheckAndNext(12.0);
  ValidCheckAndNext(13.0);
  ValidCheckAndNext(14.0);
  ValidCheckAndNext(15.0);
  REQUIRE(!comp_it.Valid());
  // Test that no matter how many times we try to get the next component entry we remain invalid.
  comp_it.Next();
  REQUIRE(!comp_it.Valid());
  comp_it.Next();
  REQUIRE(!comp_it.Valid());
  comp_it.Next();
  REQUIRE(!comp_it.Valid());
  comp_it.Next();
  REQUIRE(!comp_it.Valid());
}

TEST_CASE("Test ComponentArray")
{
  evie::ComponentArray<TestComponent1> comp_array(evie::ComponentID<TestComponent1>(0));
  evie::EntityID id_0{ 0 };
  evie::EntityID id_1{ 1 };
  evie::EntityID id_2{ 2 };
  evie::EntityID id_3{ 3 };
  evie::EntityID id_4{ 4 };
  comp_array.AddComponent(id_0, { 0 });
  comp_array.AddComponent(id_1, { 1 });
  comp_array.AddComponent(id_2, { 2 });
  comp_array.AddComponent(id_3, { 3 });
  comp_array.AddComponent(id_4, { 4 });
  auto comp_it = comp_array.GetComponentIterator();
  auto ValidCheckAndNext = [&](float val) {
    REQUIRE(comp_it.Valid());
    REQUIRE(comp_it->a == val);
    comp_it.Next();
  };
  REQUIRE_EQ(comp_array.Size(), 5);
  // Check all components exist
  ValidCheckAndNext(0.0);
  ValidCheckAndNext(1.0);
  ValidCheckAndNext(2.0);
  ValidCheckAndNext(3.0);
  ValidCheckAndNext(4.0);
  // Remove components
  comp_array.RemoveComponent(id_1);
  comp_array.RemoveComponent(id_2);
  REQUIRE_EQ(comp_array.Size(), 3);
  // Get a new iterator - Maybe we want to allow our old one to be reset as it's still valid?
  comp_it = comp_array.GetComponentIterator();
  // Check left over components exist
  ValidCheckAndNext(0.0);
  ValidCheckAndNext(3.0);
  ValidCheckAndNext(4.0);
  REQUIRE_EQ(comp_array.Size(), 3);
  // Add components back in a different order
  comp_array.AddComponent(id_2, { 2 });
  REQUIRE_EQ(comp_array.Size(), 4);
  comp_array.AddComponent(id_1, { 1 });
  REQUIRE_EQ(comp_array.Size(), 5);
  // Now get a new iterator and check that the order is 0,2,1,3,4
  // This order is because we removed 1, 2 and then put back 2, 1 and the free slots
  // would have been used up first before allocating new ones
  comp_it = comp_array.GetComponentIterator();
  ValidCheckAndNext(0.0);
  ValidCheckAndNext(2.0);
  ValidCheckAndNext(1.0);
  ValidCheckAndNext(3.0);
  ValidCheckAndNext(4.0);
  evie::EntityID id_5{ 5 };
  comp_array.AddComponent(id_5, { 5 });
  REQUIRE_EQ(comp_array.Size(), 6);
  comp_it = comp_array.GetComponentIterator();
  ValidCheckAndNext(0.0);
  ValidCheckAndNext(2.0);
  ValidCheckAndNext(1.0);
  ValidCheckAndNext(3.0);
  ValidCheckAndNext(4.0);
  ValidCheckAndNext(5.0);
}

TEST_CASE("Test ComponentManager")
{
  evie::ComponentManager comp_manager;
  evie::EntityID id_0{ 0 };
  evie::EntityID id_1{ 1 };
  evie::EntityID id_2{ 2 };
  evie::EntityID id_3{ 3 };
  evie::EntityID id_4{ 4 };
  // Register all our components first
  auto comp_id_1 = comp_manager.RegisterComponent<TestComponent1>();
  auto comp_id_2 = comp_manager.RegisterComponent<TestComponent2>();
  auto comp_id_3 = comp_manager.RegisterComponent<TestComponent3>();
  auto comp_id_4 = comp_manager.RegisterComponent<TestComponent4>();
  auto comp_id_5 = comp_manager.RegisterComponent<TestComponent5>();
  // Register components to entities
  auto RegEntityAndCheck = [&]<typename T>(evie::EntityID id, evie::ComponentID<T> comp_id, T comp) {
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
  // Now ask the component manager for specific components to iterate over
  auto comp_it_1 = comp_manager.GetComponentIterator(comp_id_1);
  REQUIRE(comp_it_1.Good());
  auto ValidCheckAndNext = []<typename T>(T& comp_it, int val) {
    REQUIRE(comp_it->Valid());
    REQUIRE((*comp_it)->a == val);
    comp_it->Next();
  };
  // Check all the components that we registered exist
  ValidCheckAndNext(comp_it_1, 1);
  ValidCheckAndNext(comp_it_1, 5);
  ValidCheckAndNext(comp_it_1, 9);
  auto comp_it_2 = comp_manager.GetComponentIterator(comp_id_2);
  REQUIRE(comp_it_2.Good());
  ValidCheckAndNext(comp_it_2, 2);
  ValidCheckAndNext(comp_it_2, 6);
  ValidCheckAndNext(comp_it_2, 11);
  auto comp_it_3 = comp_manager.GetComponentIterator(comp_id_3);
  REQUIRE(comp_it_3.Good());
  ValidCheckAndNext(comp_it_3, 3);
  ValidCheckAndNext(comp_it_3, 7);
  ValidCheckAndNext(comp_it_3, 12);
  auto comp_it_4 = comp_manager.GetComponentIterator(comp_id_4);
  REQUIRE(comp_it_4.Good());
  ValidCheckAndNext(comp_it_4, 4);
  ValidCheckAndNext(comp_it_4, 8);
  ValidCheckAndNext(comp_it_4, 10);
  // Now inform the component manager that the entity 0 has been destroyed.
  // All of it's component entries should have disappeared.
  comp_manager.EntityDestroyed(id_0);
  comp_it_1 = comp_manager.GetComponentIterator(comp_id_1);
  REQUIRE(comp_it_1.Good());
  ValidCheckAndNext(comp_it_1, 5);
  ValidCheckAndNext(comp_it_1, 9);
  comp_it_2 = comp_manager.GetComponentIterator(comp_id_2);
  REQUIRE(comp_it_2.Good());
  ValidCheckAndNext(comp_it_2, 6);
  ValidCheckAndNext(comp_it_2, 11);
  comp_it_3 = comp_manager.GetComponentIterator(comp_id_3);
  REQUIRE(comp_it_3.Good());
  ValidCheckAndNext(comp_it_3, 7);
  ValidCheckAndNext(comp_it_3, 12);
  comp_it_4 = comp_manager.GetComponentIterator(comp_id_4);
  REQUIRE(comp_it_4.Good());
  ValidCheckAndNext(comp_it_4, 8);
  ValidCheckAndNext(comp_it_4, 10);
  // Now remove just entity 1 TestComponent2
  comp_manager.RemoveComponent(id_1, comp_id_2);
  comp_it_2 = comp_manager.GetComponentIterator(comp_id_2);
  REQUIRE(comp_it_2.Good());
  ValidCheckAndNext(comp_it_2, 11);
  // And the same with entity 4
  comp_manager.RemoveComponent(id_4, comp_id_2);
  comp_it_2 = comp_manager.GetComponentIterator(comp_id_2);
  REQUIRE(comp_it_2.Good());
  // We removed all entities for this component.
  REQUIRE_FALSE(comp_it_2->Valid());
}