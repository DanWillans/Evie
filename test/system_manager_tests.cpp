#include <doctest/doctest.h>

#include "evie/ids.h"
#include "evie/ecs/component_manager.hpp"
#include "evie/ecs/entity_manager.hpp"
#include "evie/ecs/system_manager.hpp"

using namespace evie;

TEST_CASE("Test system manager")
{
  struct System1 : public System
  {
    void Update(const float& delta_time){}
  };
  struct System2 : public System
  {
    void Update(const float& delta_time){}
  };
  struct System3 : public System
  {
    void Update(const float& delta_time){}
  };
  struct System4 : public System
  {
    void Update(const float& delta_time){}
  };
  struct System5 : public System
  {
    void Update(const float& delta_time){}
  };
  struct TestComponent1
  {
  };
  struct TestComponent2
  {
  };
  struct TestComponent3
  {
  };
  struct TestComponent4
  {
  };
  struct TestComponent5
  {
  };

  // Create entities
  EntityManager ent_man;
  auto ent_id_1 = ent_man.CreateEntity();
  REQUIRE(ent_id_1.Good());
  auto ent_id_2 = ent_man.CreateEntity();
  REQUIRE(ent_id_2.Good());
  auto ent_id_3 = ent_man.CreateEntity();
  REQUIRE(ent_id_3.Good());
  auto ent_id_4 = ent_man.CreateEntity();
  REQUIRE(ent_id_4.Good());
  auto ent_id_5 = ent_man.CreateEntity();
  REQUIRE(ent_id_5.Good());


  // Create components
  ComponentManager comp_man;
  auto comp_id_1 = comp_man.RegisterComponent<TestComponent1>();
  auto comp_id_2 = comp_man.RegisterComponent<TestComponent2>();
  auto comp_id_3 = comp_man.RegisterComponent<TestComponent3>();
  auto comp_id_4 = comp_man.RegisterComponent<TestComponent4>();
  auto comp_id_5 = comp_man.RegisterComponent<TestComponent5>();

  // Create systems
  SystemManager sys_man(&comp_man, &ent_man);
  SystemSignature signature_1;
  signature_1.SetComponent(comp_id_1);
  signature_1.SetComponent(comp_id_2);
  auto sys_id_1 = sys_man.RegisterSystem<System1>(signature_1);

  SystemSignature signature_2;
  signature_2.SetComponent(comp_id_1);
  signature_2.SetComponent(comp_id_2);
  signature_2.SetComponent(comp_id_3);
  auto sys_id_2 = sys_man.RegisterSystem<System2>(signature_2);

  SystemSignature signature_3;
  signature_3.SetComponent(comp_id_1);
  signature_3.SetComponent(comp_id_2);
  signature_3.SetComponent(comp_id_3);
  auto sys_id_3 = sys_man.RegisterSystem<System3>(signature_3);

  SystemSignature signature_4;
  signature_4.SetComponent(comp_id_1);
  auto sys_id_4 = sys_man.RegisterSystem<System4>(signature_4);

  SystemSignature signature_5;
  signature_5.SetComponent(comp_id_1);
  signature_5.SetComponent(comp_id_2);
  signature_5.SetComponent(comp_id_3);
  signature_5.SetComponent(comp_id_4);
  signature_5.SetComponent(comp_id_5);
  auto sys_id_5 = sys_man.RegisterSystem<System5>(signature_5);

  // Inform system manager about an entity signature
  sys_man.EntitySignatureChanged(*ent_id_1, signature_1);
  sys_man.EntitySignatureChanged(*ent_id_2, signature_1);
  sys_man.EntitySignatureChanged(*ent_id_3, signature_1);
  sys_man.EntitySignatureChanged(*ent_id_4, signature_1);
  sys_man.EntitySignatureChanged(*ent_id_5, signature_1);

  // Check Systems have the correct entities
  const auto& system_1 = sys_man.GetSystem(sys_id_1);
  REQUIRE_EQ(system_1.entities.size(), 5);
  const auto& system_2 = sys_man.GetSystem(sys_id_2);
  REQUIRE_EQ(system_2.entities.size(), 0);
  const auto& system_3 = sys_man.GetSystem(sys_id_3);
  REQUIRE_EQ(system_3.entities.size(), 0);
  const auto& system_4 = sys_man.GetSystem(sys_id_4);
  REQUIRE_EQ(system_4.entities.size(), 5);
  const auto& system_5 = sys_man.GetSystem(sys_id_5);
  REQUIRE_EQ(system_5.entities.size(), 0);

  // Let's change the signature of some entities and recheck the systems
  sys_man.EntitySignatureChanged(*ent_id_1, signature_5);
  sys_man.EntitySignatureChanged(*ent_id_2, signature_4);

  // Check Systems have the correct entities
  REQUIRE_EQ(system_1.entities.size(), 4);
  REQUIRE_EQ(system_2.entities.size(), 1);
  REQUIRE_EQ(system_3.entities.size(), 1);
  REQUIRE_EQ(system_4.entities.size(), 5);
  REQUIRE_EQ(system_5.entities.size(), 1);
}
