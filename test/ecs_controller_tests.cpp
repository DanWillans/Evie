#include "doctest/doctest.h"

#include "evie/ecs/ecs_controller.hpp"

using namespace evie;

// NOLINTBEGIN

TEST_CASE("Test ECS Controller")
{
  struct TestComponent1
  {
  };
  struct TestComponent2
  {
  };
  struct TestComponent3
  {
  };
  struct System1 : public System
  {
    void Update(const float& delta_time) override { std::ignore = delta_time; }
  };
  struct System2 : public System
  {
    void Update(const float& delta_time) override { std::ignore = delta_time; }
  };
  struct System3 : public System
  {
    void Update(const float& delta_time) override { std::ignore = delta_time; }
  };
  ECSController ecs;
  auto entity_1 = ecs.CreateEntity();
  REQUIRE(entity_1);
  auto entity_2 = ecs.CreateEntity();
  REQUIRE(entity_2);
  auto entity_3 = ecs.CreateEntity();
  REQUIRE(entity_3);
  // Make sure all entities are unique
  REQUIRE(*entity_1 != *entity_2);
  REQUIRE(*entity_2 != *entity_3);
  REQUIRE(*entity_1 != *entity_3);

  // Register component types
  auto test_comp_id_1 = ecs.RegisterComponent<TestComponent1>();
  auto test_comp_id_2 = ecs.RegisterComponent<TestComponent2>();
  auto test_comp_id_3 = ecs.RegisterComponent<TestComponent3>();

  // Register systems and their signatures
  SystemSignature signature_1;
  signature_1.SetComponent(test_comp_id_1);
  signature_1.SetComponent(test_comp_id_2);
  auto system_id_1 = ecs.RegisterSystem<System1>(signature_1);

  SystemSignature signature_2;
  signature_2.SetComponent(test_comp_id_1);
  auto system_id_2 = ecs.RegisterSystem<System2>(signature_2);

  SystemSignature signature_3;
  signature_3.SetComponent(test_comp_id_1);
  signature_3.SetComponent(test_comp_id_2);
  signature_3.SetComponent(test_comp_id_3);
  auto system_id_3 = ecs.RegisterSystem<System3>(signature_3);

  // Now we can use Entity objects to add components for that entity
  auto err = entity_1->AddComponent(test_comp_id_1, {});
  REQUIRE(err);
  err = entity_1->AddComponent(test_comp_id_2, {});
  REQUIRE(err);
  err = entity_1->AddComponent(test_comp_id_3, {});
  REQUIRE(err);

  err = entity_2->AddComponent(test_comp_id_1, {});
  REQUIRE(err);
  err = entity_2->AddComponent(test_comp_id_2, {});
  REQUIRE(err);

  err = entity_3->AddComponent(test_comp_id_1, {});
  REQUIRE(err);
  err = entity_3->AddComponent(test_comp_id_3, {});
  REQUIRE(err);

  // Check systems have the correct entities
  const auto& sys_1 = ecs.GetSystem(system_id_1);
  const auto& sys_2 = ecs.GetSystem(system_id_2);
  const auto& sys_3 = ecs.GetSystem(system_id_3);

  // Check system entity sizes
  REQUIRE_EQ(sys_1.entities.size(), 2);
  REQUIRE_EQ(sys_2.entities.size(), 3);
  REQUIRE_EQ(sys_3.entities.size(), 1);

  // Remove components on entities
  printf("blah");
  err = entity_1->RemoveComponent(test_comp_id_2);
  printf("blah");
  REQUIRE(err);
  err = entity_1->RemoveComponent(test_comp_id_3);
  printf("blah");
  REQUIRE(err);
  err = entity_3->RemoveComponent(test_comp_id_3);
  REQUIRE(err);
  printf("blah");

  // Check systems have correct entities again
  REQUIRE_EQ(sys_1.entities.size(), 1);
  REQUIRE_EQ(sys_2.entities.size(), 3);
  REQUIRE_EQ(sys_3.entities.size(), 0);

  REQUIRE_EQ(ecs.EntityCount(), 3);

  // Destroy entity
  printf("blah\n");
  entity_1->Destroy();
  printf("blah");

  // Check systems have correct entities again
  REQUIRE_EQ(sys_1.entities.size(), 1);
  REQUIRE_EQ(sys_2.entities.size(), 2);
  REQUIRE_EQ(sys_3.entities.size(), 0);

  printf("blah");
  REQUIRE_EQ(ecs.EntityCount(), 2);

  printf("blah");
  // Destroy entity
  entity_2->Destroy();

  printf("blah");
  // Check systems have correct entities again
  REQUIRE_EQ(sys_1.entities.size(), 0);
  REQUIRE_EQ(sys_2.entities.size(), 1);
  REQUIRE_EQ(sys_3.entities.size(), 0);

  REQUIRE_EQ(ecs.EntityCount(), 1);

  // Destroy entity
  printf("blah");
  entity_3->Destroy();

  printf("blah");
  // Check systems have correct entities again
  REQUIRE_EQ(sys_1.entities.size(), 0);
  REQUIRE_EQ(sys_2.entities.size(), 0);
  REQUIRE_EQ(sys_3.entities.size(), 0);

  REQUIRE_EQ(ecs.EntityCount(), 0);
}
// NOLINTEND