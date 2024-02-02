#include "evie/ecs/ecs_constants.hpp"
#include <doctest/doctest.h>

#include <evie/ecs/entity_manager.hpp>
#include <evie/ids.h>


TEST_CASE("Test entity manager")
{
  evie::EntityManager entity_manager;
  auto entity_id_0 = entity_manager.CreateEntity();
  auto entity_id_1 = entity_manager.CreateEntity();
  auto entity_id_2 = entity_manager.CreateEntity();
  auto entity_id_3 = entity_manager.CreateEntity();
  REQUIRE_EQ(entity_manager.EntityCount(), 4);
}

TEST_CASE("Test entity manager limits")
{
  evie::EntityManager entity_manager;
  evie::Result<evie::EntityID> result{ evie::Error("Nope") };
  for (size_t i = 0; i < evie::MAX_ENTITY_COUNT; ++i) {
    result = entity_manager.CreateEntity();
    REQUIRE(result.Good());
  }
  auto bad_result = entity_manager.CreateEntity();
  REQUIRE(bad_result.Bad());
  entity_manager.DestroyEntity(*result);
  result = entity_manager.CreateEntity();
  REQUIRE(result.Good());
}