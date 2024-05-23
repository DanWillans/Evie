#include <doctest/doctest.h>

#include "evie/ecs/entity_manager.hpp"
#include "evie/ecs/ecs_constants.hpp"
#include "evie/ids.h"
#include "evie/error.h"
#include "evie/result.h"

using namespace evie;

TEST_CASE("Test entity manager")
{
  EntityManager entity_manager;
  auto entity_id_0 = entity_manager.CreateEntity();
  auto entity_id_1 = entity_manager.CreateEntity();
  auto entity_id_2 = entity_manager.CreateEntity();
  auto entity_id_3 = entity_manager.CreateEntity();
  REQUIRE_EQ(entity_manager.EntityCount(), 4);
  entity_manager.DestroyEntity(*entity_id_0);
  entity_manager.DestroyEntity(*entity_id_1);
  entity_manager.DestroyEntity(*entity_id_2);
  entity_manager.DestroyEntity(*entity_id_3);
  REQUIRE_EQ(entity_manager.EntityCount(), 0);
}

TEST_CASE("Test entity manager limits")
{
  EntityManager entity_manager;
  Result<EntityID> result{ Error("Nope") };
  for (size_t i = 0; i < MAX_ENTITY_COUNT; ++i) {
    result = entity_manager.CreateEntity();
    REQUIRE(result.Good());
  }
  auto bad_result = entity_manager.CreateEntity();
  REQUIRE(bad_result.Bad());
  entity_manager.DestroyEntity(*result);
  result = entity_manager.CreateEntity();
  REQUIRE(result.Good());
}
