#include "game_layer.hpp"
#include "components.hpp"
#include "follow_system.hpp"
#include "physics_system.hpp"
#include "projectile_system.hpp"

#include <GLFW/glfw3.h>

#include <dandan_system.hpp>
#include <evie/default_models.h>
#include <evie/ecs/components/Transform.hpp>
#include <evie/ecs/components/mesh_component.hpp>
#include <evie/ecs/ecs_controller.hpp>
#include <evie/ecs/system_signature.hpp>
#include <evie/error.h>
#include <evie/events.h>
#include <evie/input.h>
#include <evie/input_manager.h>
#include <evie/key_events.h>
#include <evie/mouse_events.h>
#include <evie/shader.h>
#include <evie/texture.h>
#include <evie/vertex_buffer.h>
#include <evie/window.h>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <imgui.h>
#include <numbers>

evie::Error
  GameLayer::Initialise(evie::IInputManager* input_manager, evie::ECSController* ecs_controller, evie::IWindow* window)
{
  evie::Error err = evie::Error::OK();
  // Initialise our variables
  input_manager_ = input_manager;
  if (input_manager_ == nullptr) {
    return evie::Error{ "Invalid input manager" };
  }

  ecs_ = ecs_controller;
  if (ecs_ == nullptr) {
    return evie::Error{ "Invalid ECS" };
  }

  window_ = window;
  if (window_ == nullptr) {
    return evie::Error{ "Invalid window" };
  }

  constexpr float map_scale = 50.0F;

  // Let's use ECS to add data to our models
  // Register our componenets
  mesh_cid_ = ecs_->RegisterComponent<evie::MeshComponent>();
  transform_cid_ = ecs_->RegisterComponent<evie::TransformComponent>();
  follow_target_cid_ = ecs_->RegisterComponent<FollowTargetComponent>();
  follower_cid_ = ecs_->RegisterComponent<FollowerComponent>();
  projectile_cid_ = ecs_->RegisterComponent<ProjectileComponent>();
  velocity_cid_ = ecs_->RegisterComponent<VelocityComponent>();
  enemy_cid_ = ecs_->RegisterComponent<EnemyComponent>();

  // Register our render
  evie::SystemSignature signature;
  signature.SetComponent(mesh_cid_);
  signature.SetComponent(transform_cid_);
  auto sys_id = ecs_->RegisterSystem<Renderer>(signature);
  renderer_ = &(ecs_->GetSystem(sys_id));
  renderer_->Initialise(mesh_cid_, transform_cid_, &player_camera_, window_);

  // Register our follow system
  evie::SystemSignature follow_signature;
  follow_signature.SetComponent(follower_cid_);
  follow_signature.SetComponent(velocity_cid_);
  follow_signature.SetComponent(transform_cid_);
  auto fol_sys_id = ecs_->RegisterSystem<FollowSystem>(
    follow_signature, follow_target_cid_, follower_cid_, transform_cid_, velocity_cid_);
  follower_system_ = &(ecs_->GetSystem(fol_sys_id));
  follower_system_->Initialise();

  // Register our physics system
  evie::SystemSignature physics_signature;
  physics_signature.SetComponent(velocity_cid_);
  auto phys_sys_id = ecs_->RegisterSystem<PhysicsSystem>(physics_signature, velocity_cid_, transform_cid_);
  physics_system_ = &(ecs_->GetSystem(phys_sys_id));

  // Register our DanDan system
  evie::SystemSignature dan_dan_signature;
  dan_dan_signature.SetComponent(enemy_cid_);
  auto enemy_sys_id = ecs_->RegisterSystem<DanDanSystem>(dan_dan_signature,
    enemy_cid_,
    mesh_cid_,
    transform_cid_,
    follower_cid_,
    projectile_cid_,
    follow_target_cid_,
    velocity_cid_,
    ecs_,
    map_scale);
  dandan_system_ = &(ecs_->GetSystem(enemy_sys_id));
  if (err.Good()) {
    err = dandan_system_->Initialise();
  }

  if (err.Good()) {
    err = SetupPlayer(map_scale);
  }

  // Register our projectile system - After player entity is created.
  evie::SystemSignature project_signature;
  project_signature.SetComponent(projectile_cid_);
  auto projectile_sys_id = ecs_->RegisterSystem<ProjectileSystem>(project_signature,
    input_manager_,
    ecs_,
    mesh_cid_,
    transform_cid_,
    projectile_cid_,
    velocity_cid_,
    player_entity_,
    map_scale);
  projectile_system_ = &(ecs_->GetSystem(projectile_sys_id));
  if (err.Good()) {
    err = projectile_system_->Initialise();
  }

  if (err.Good()) {
    err = SetupFloor(map_scale);
  }

  constexpr float skybox_scale = 10.0F;
  if (err.Good()) {
    err = SetupSkybox(map_scale * skybox_scale);
  }

  if (err.Good()) {
    err = SetupWalls(map_scale);
  }

  // Disable cursor
  if (!enable_cursor_) {
    window_->DisableCursor();
  }

  return err;
}

void GameLayer::OnUpdate()
{
  // Calculate some delta times between last OnUpdate call
  // TODO - Would it be nice if we didn't have to handle delta_time ourselves?
  auto current_frame = static_cast<float>(glfwGetTime());
  float delta_time = current_frame - last_frame_;
  last_frame_ = current_frame;

  HandlePlayerCameraMovement(delta_time);

  // Update follow system
  follower_system_->FollowOn(follow_on_);
  follower_system_->UpdateSystem(delta_time);

  // Update dandan system
  dandan_system_->UpdateSystem(delta_time);

  // Update projectile system
  projectile_system_->UpdateSystem(delta_time);

  // Update physics system
  physics_system_->UpdateSystem(delta_time);
}

void GameLayer::OnRender() { renderer_->UpdateSystem(0.0F); }

void GameLayer::OnEvent(evie::Event& event)
{
  if (event.GetEventType() == evie::EventType::MouseMoved) {
    // Don't rotate the camera if the cursor is enabled
    const auto* const mouse_event = event.Cast<evie::MouseMovementEvent>();
    player_camera_.Rotate(mouse_event->GetMousePosition(), !enable_cursor_);
    event.handled = true;
    return;
  }

  if (event.GetEventType() == evie::EventType::KeyPressed) {
    const auto* const key_event = event.Cast<evie::KeyPressedEvent>();
    // Check if we should enable the cursor or not
    if (key_event->IsKeyCode(evie::KeyCode::Escape)) {
      enable_cursor_ = !enable_cursor_;
      if (enable_cursor_) {
        window_->EnableCursor();
      } else {
        window_->DisableCursor();
      }
      event.handled = true;
      return;
    }
    // Toggle sprint
    if (key_event->IsKeyCode(evie::KeyCode::LeftShift)) {
      sprint_ = !sprint_;
      if (sprint_) {
        player_camera_.camera_speed = SprintPlayerSpeed;
      } else {
        player_camera_.camera_speed = DefaultPlayerSpeed;
      }
      event.handled = true;
      return;
    }
    if (key_event->IsKeyCode(evie::KeyCode::N)) {
      follow_on_ = !follow_on_;
      event.handled = true;
      return;
    }
  }

  if (event.GetEventType() == evie::EventType::MouseButtonPressed) {
    const auto* const mouse_event = event.Cast<evie::MousePressedEvent>();
    if (mouse_event->GetMouseButton() == evie::MouseCode::ButtonLeft) {
      projectile_system_->MousePressed();
      event.handled = true;
      return;
    }
  }
}

void GameLayer::HandlePlayerCameraMovement(float delta_time)
{
  // Handle camera translation on update
  if (input_manager_->IsKeyPressed(evie::KeyCode::W)) {
    player_camera_.MoveForwards(delta_time);
  }
  if (input_manager_->IsKeyPressed(evie::KeyCode::S)) {
    player_camera_.MoveBackwards(delta_time);
  }
  if (input_manager_->IsKeyPressed(evie::KeyCode::A)) {
    player_camera_.MoveLeft(delta_time);
  }
  if (input_manager_->IsKeyPressed(evie::KeyCode::D)) {
    player_camera_.MoveRight(delta_time);
  }

  // Update the player position
  auto& transform = player_entity_->GetComponent(transform_cid_);
  // At the minute just copy the player camera position
  transform.position = player_camera_.GetPosition();
  // Calculate the rotation of the player by taking the camera angles and converting into quaternions.
  const auto& camera_euler = player_camera_.GetEulerAngles();
  // NOLINTNEXTLINE(*-union-access)
  evie::quat yaw_q = glm::angleAxis(glm::radians(camera_euler.y), evie::vec3{ 0.0F, 1.0F, 0.0F });
  // NOLINTNEXTLINE(*-union-access)
  evie::quat pitch_q = glm::angleAxis(glm::radians(camera_euler.x), evie::vec3{ 1.0F, 0.0F, 0.0F });
  transform.rotation = yaw_q * pitch_q;
}

evie::Error GameLayer::SetupFloor(float map_scale)
{
  evie::Error err = evie::Error::OK();
  // Let's make a floor
  // We're going to do this by creating a 2d plane and mapping a texture to it.
  // We can then scale the plane in size to increase the floor area.
  // We'll map the texture to a tenth of the size of the floor.
  constexpr float texture_repeat = 10.0F;
  const float texture_scale = map_scale / texture_repeat;
  // clang-format off
  const std::vector<float> floor_model = {
    // x    y     z    u    v
     0.5F, 0.0F, 0.5F, texture_scale * 1.0F, texture_scale * 1.0F, // top right
     0.5F, 0.0F, -0.5F, texture_scale * 1.0F, 0.0,// bottom right
    -0.5F, 0.0F, 0.5F, 0.0, texture_scale * 1.0F, // top left 
     0.5F, 0.0F, -0.5F, texture_scale * 1.0F, 0.0, // bottom right
    -0.5F, 0.0F, -0.5F, 0.0, 0.0, // bottom left
    -0.5F, 0.0F, 0.5F, 0.0, texture_scale * 1.0F  // top left
  };
  // clang-format on

  // Setup the camera
  // Camera Height == player height
  player_camera_.ResetCameraPosition({ 0.0, 1.0, 0.0 });
  player_camera_.camera_speed = DefaultPlayerSpeed;

  // Vertex Shader
  if (err.Good()) {
    err = floor_vertex_shader_.Initialise(R"(C:\Users\willa\devel\Evie\shaders\vertex_shader.vs)");
  }

  // Fragment Shader
  if (err.Good()) {
    err = floor_fragment_shader_.Initialise(R"(C:\Users\willa\devel\Evie\shaders\fragment_shader.fs)");
  }

  // Floor texture
  if (err.Good()) {
    err = floor_texture_.Initialise(
      R"(C:\Users\willa\devel\Evie\out\install\windows-msvc-debug-developer-mode\assets\textures\stone-wall.jpg)",
      false,
      evie::TextureWrapping::MirroredRepeat);
  }

  evie::MeshComponent floor_mesh_component;
  // We need to create the layout for our model/vertex data
  evie::BufferLayout layout;
  layout.stride = 5;// NOLINT
  layout.type = evie::VertexDataType::Float;
  layout.layout_sizes = { 3, 2 };
  if (err.Good()) {
    // Initialise the vertex buffer
    err = floor_mesh_component.model_data.Initialise(floor_model, layout);
  }
  if (err.Good()) {
    // Associate the vertex buffer with the vertex array
    floor_mesh_component.vertex_array.Initialise();
    err = floor_mesh_component.vertex_array.AssociateVertexBuffer(floor_mesh_component.model_data);
  }
  if (err.Good()) {
    // Initialise the shader program with our vert/frag shaders
    err = floor_mesh_component.shader_program.Initialise(&floor_vertex_shader_, &floor_fragment_shader_);
  }
  // Now setup the texture slots and bind them to our shader program.
  floor_mesh_component.shader_program.Use();
  floor_mesh_component.shader_program.SetInt("Texture1", 0);
  floor_mesh_component.texture = floor_texture_;

  // Create floor
  auto floor_entity = ecs_->CreateEntity();
  if (err.Good()) {
    err = floor_entity->AddComponent(mesh_cid_, floor_mesh_component);
    if (err.Good()) {
      evie::TransformComponent transform;
      transform.scale = { map_scale * 1.0F, 1.0F, map_scale * 1.0F };
      err = floor_entity->AddComponent(transform_cid_, transform);
    }
  }

  return err;
}

evie::Error GameLayer::SetupSkybox(float map_scale)
{
  evie::Error err = evie::Error::OK();

  // Sky box texture
  evie::Texture2D sky_texture;
  if (err.Good()) {
    err = sky_texture.Initialise(
      R"(C:\Users\willa\devel\Evie\out\install\windows-msvc-debug-developer-mode\assets\textures\skybox.png)", true);
  }

  evie::VertexShader vert_shader;
  evie::FragmentShader frag_shader;
  // Vertex Shader
  if (err.Good()) {
    err = vert_shader.Initialise(R"(C:\Users\willa\devel\Evie\shaders\vertex_shader.vs)");
  }

  // Fragment Shader
  if (err.Good()) {
    err = frag_shader.Initialise(R"(C:\Users\willa\devel\Evie\shaders\fragment_shader.fs)");
  }

  // Create skybox entity
  auto entity = ecs_->CreateEntity();
  if (err.Bad()) {
    err = entity.Error();
  }

  // clang-format off
std::vector<float> sky_cube {
  //NOLINTBEGIN
    -0.5F, -0.5f, -0.5f,  0.0f, 1.0f/3.0f,
    0.5f, -0.5f, -0.5f,  0.25f, 1.0f/3.0f,
    0.5f,  0.5f, -0.5f,  0.25f, 2.0f/3.0f,
    0.5f,  0.5f, -0.5f,  0.25f, 2.0f/3.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 2.0f/3.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f/3.0f,

    -0.5f, -0.5f,  0.5f,  0.75f, 1.0f/3.0f,
    0.5f, -0.5f,  0.5f,  0.5f, 1.0f/3.0f,
    0.5f,  0.5f,  0.5f,  0.5f, 2.0f/3.0f,
    0.5f,  0.5f,  0.5f,  0.5f, 2.0f/3.0f,
    -0.5f,  0.5f,  0.5f,  0.75f, 2.0f/3.0f,
    -0.5f, -0.5f,  0.5f,  0.75f, 1.0f/3.0f,

    -0.5f,  0.5f,  0.5f,   0.75f, 2.0f/3.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 2.0f/3.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 1.0f/3.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 1.0f/3.0f,
    -0.5f, -0.5f,  0.5f,   0.75f, 1.0f/3.0f,
    -0.5f,  0.5f,  0.5f,   0.75f, 2.0f/3.0f,

    0.5f,  0.5f,  0.5f,  0.5f, 2.0f/3.0f,
    0.5f,  0.5f, -0.5f,  0.25f, 2.0f/3.0f,
    0.5f, -0.5f, -0.5f,  0.25f, 1.0f/3.0f,
    0.5f, -0.5f, -0.5f,  0.25f, 1.0f/3.0f,
    0.5f, -0.5f,  0.5f,  0.5f, 1.0f/3.0f,
    0.5f,  0.5f,  0.5f,  0.5f, 2.0f/3.0f,

    -0.5f, -0.5f, -0.5f,  0.25f, 0.0f,
    0.5f, -0.5f, -0.5f,  0.25f, 1.0f/3.0f,
    0.5f, -0.5f,  0.5f,  0.5f, 1.0f/3.0f,
    0.5f, -0.5f,  0.5f,  0.5f, 1.0f/3.0f,
    -0.5f, -0.5f,  0.5f,  0.5f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.25f, 0.0f,

    -0.5f,  0.5f, -0.5f,  0.25f, 1.0f,
    0.5f,  0.5f, -0.5f,  0.25f, 2.0F/3.0f,
    0.5f,  0.5f,  0.5f,  0.5f, 2.0F/3.0f,
    0.5f,  0.5f,  0.5f,  0.5f, 2.0f/3.0f,
    -0.5f,  0.5f,  0.5f,  0.5f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.25f, 1.0f
    //NOLINTEND
};
  // clang-format on

  evie::MeshComponent mesh_component;
  evie::BufferLayout layout;
  layout.stride = 5;// NOLINT
  layout.type = evie::VertexDataType::Float;
  layout.layout_sizes = { 3, 2 };

  if (err.Good()) {
    err = mesh_component.model_data.Initialise(sky_cube, layout);
  }

  if (err.Good()) {
    mesh_component.vertex_array.Initialise();
    err = mesh_component.vertex_array.AssociateVertexBuffer(mesh_component.model_data);
  }

  if (err.Good()) {
    err = mesh_component.shader_program.Initialise(&vert_shader, &frag_shader);
  }

  if (err.Good()) {
    mesh_component.shader_program.Use();
    mesh_component.shader_program.SetInt("Texture1", 0);
    mesh_component.texture = sky_texture;

    err = entity->AddComponent(mesh_cid_, mesh_component);
    if (err.Good()) {
      evie::TransformComponent transform;
      transform.scale = { map_scale, map_scale, map_scale };
      err = entity->AddComponent(transform_cid_, transform);
    }
  }
  return err;
}

evie::Error GameLayer::SetupWalls(float map_scale)
{
  evie::Error err = evie::Error::OK();

  // We're going to make walls as 2d planes with texture, the same as the floor.
  float texture_scale = map_scale;
  // clang-format off
  const std::vector<float> wall_model = {
    // x    y     z    u    v
    0.5F,   0.5F, 0.0F, texture_scale * 1.0F,  1.0F,// top right
    0.5F,  -0.5F, 0.0F, texture_scale * 1.0F, 0.0,// bottom right
    -0.5F,  0.5F, 0.0F, 0.0, 1.0F,// top left
    0.5F,  -0.5F, 0.0F, texture_scale * 1.0F, 0.0,// bottom right
    -0.5F, -0.5F, 0.0F, 0.0, 0.0,// bottom left
    -0.5F,  0.5F, 0.0F, 0.0, 1.0F// top left
  };
  // clang-format on

  // Vertex Shader
  evie::VertexShader vert_shader;
  evie::FragmentShader frag_shader;
  if (err.Good()) {
    err = vert_shader.Initialise(R"(C:\Users\willa\devel\Evie\shaders\vertex_shader.vs)");
  }

  // Fragment Shader
  if (err.Good()) {
    err = frag_shader.Initialise(R"(C:\Users\willa\devel\Evie\shaders\fragment_shader.fs)");
  }

  evie::Texture2D tex;
  // Floor texture
  if (err.Good()) {
    err = tex.Initialise(
      R"(C:\Users\willa\devel\Evie\out\install\windows-msvc-debug-developer-mode\assets\textures\my-wall2.png)");
  }

  evie::MeshComponent wall_component;
  // We need to create the layout for our model/vertex data
  evie::BufferLayout layout;
  layout.stride = 5;// NOLINT
  layout.type = evie::VertexDataType::Float;
  layout.layout_sizes = { 3, 2 };
  if (err.Good()) {
    // Initialise the vertex buffer
    err = wall_component.model_data.Initialise(wall_model, layout);
  }
  if (err.Good()) {
    // Associate the vertex buffer with the vertex array
    wall_component.vertex_array.Initialise();
    err = wall_component.vertex_array.AssociateVertexBuffer(wall_component.model_data);
  }
  if (err.Good()) {
    // Initialise the shader program with our vert/frag shaders
    err = wall_component.shader_program.Initialise(&vert_shader, &frag_shader);
  }
  // Now setup the texture slots and bind them to our shader program.
  wall_component.shader_program.Use();
  wall_component.shader_program.SetInt("Texture1", 0);
  wall_component.texture = tex;

  constexpr float wall_height_offset = 0.5F;
  const float wall_offset = map_scale / 2.0F;
  constexpr float half_cover_quat = 2.0F;

  // Create wall infront
  auto floor_entity_infront = ecs_->CreateEntity();
  if (err.Good()) {
    err = floor_entity_infront->AddComponent(mesh_cid_, wall_component);
    if (err.Good()) {
      evie::TransformComponent transform;
      transform.scale = { map_scale * 1.0F, 1.0F, map_scale * 1.0F };
      // transform the wall up by half the height.
      // NOLINTNEXTLINE(*-union-access)
      transform.position.y = wall_height_offset;
      // transform the wall to the front of the map.
      // NOLINTNEXTLINE(*-union-access)
      transform.position.z = -wall_offset;
      err = floor_entity_infront->AddComponent(transform_cid_, transform);
    }
  }

  // Create wall on left side
  auto floor_entity_left = ecs_->CreateEntity();
  if (err.Good()) {
    err = floor_entity_left->AddComponent(mesh_cid_, wall_component);
    if (err.Good()) {
      evie::TransformComponent transform;
      transform.scale = { map_scale * 1.0F, 1.0F, map_scale * 1.0F };
      // transform the wall up by half the height.
      // NOLINTNEXTLINE(*-union-access)
      transform.position.y = wall_height_offset;
      // transform the wall to the left of the map
      // NOLINTNEXTLINE(*-union-access)
      transform.position.x = -wall_offset;
      transform.rotation = glm::angleAxis(
        static_cast<float>(static_cast<float>(std::numbers::pi) / half_cover_quat), glm::vec3{ 0.0F, 1.0F, 0.0F });
      err = floor_entity_left->AddComponent(transform_cid_, transform);
    }
  }

  // Create wall on right side
  auto floor_entity_right = ecs_->CreateEntity();
  if (err.Good()) {
    err = floor_entity_right->AddComponent(mesh_cid_, wall_component);
    if (err.Good()) {
      evie::TransformComponent transform;
      transform.scale = { map_scale * 1.0F, 1.0F, map_scale * 1.0F };
      // transform the wall up by half the height.
      // NOLINTNEXTLINE(*-union-access)
      transform.position.y = wall_height_offset;
      // transform the wall to the right of the map
      // NOLINTNEXTLINE(*-union-access)
      transform.position.x = wall_offset;
      transform.rotation = glm::angleAxis(
        static_cast<float>(static_cast<float>(std::numbers::pi) / half_cover_quat), glm::vec3{ 0.0F, 1.0F, 0.0F });
      err = floor_entity_right->AddComponent(transform_cid_, transform);
    }
  }

  // Create wall behind
  auto floor_entity_behind = ecs_->CreateEntity();
  if (err.Good()) {
    err = floor_entity_behind->AddComponent(mesh_cid_, wall_component);
    if (err.Good()) {
      evie::TransformComponent transform;
      transform.scale = { map_scale * 1.0F, 1.0F, map_scale * 1.0F };
      // transform the wall up by half the height.
      // NOLINTNEXTLINE(*-union-access)
      transform.position.y = wall_height_offset;
      // transform the wall to the front of the map.
      // NOLINTNEXTLINE(*-union-access)
      transform.position.z = wall_offset;
      err = floor_entity_behind->AddComponent(transform_cid_, transform);
    }
  }

  return err;
}

evie::Error GameLayer::SetupDanDan()
{
  evie::Error err = evie::Error::OK();
  constexpr evie::vec3 dandan_scale{ 2.0F, 2.0F, 2.0F };
  constexpr float dandan_height_offset{ 1.5F };

  // Create DanDan box
  // Vertex Shader
  evie::VertexShader vert_shader;
  evie::FragmentShader frag_shader;
  if (err.Good()) {
    err = vert_shader.Initialise(R"(C:\Users\willa\devel\Evie\shaders\vertex_shader.vs)");
  }

  // Fragment Shader
  if (err.Good()) {
    err = frag_shader.Initialise(R"(C:\Users\willa\devel\Evie\shaders\fragment_shader.fs)");
  }

  evie::Texture2D tex;
  // Floor texture
  if (err.Good()) {
    err = tex.Initialise(
      R"(C:\Users\willa\devel\Evie\out\install\windows-msvc-debug-developer-mode\assets\textures\dandan.png)");
  }

  evie::MeshComponent wall_component;
  // We need to create the layout for our model/vertex data
  evie::BufferLayout layout;
  layout.stride = 5;// NOLINT
  layout.type = evie::VertexDataType::Float;
  layout.layout_sizes = { 3, 2 };
  if (err.Good()) {
    // Initialise the vertex buffer
    err = wall_component.model_data.Initialise(evie::default_models::cube_texture_up_right, layout);
  }
  if (err.Good()) {
    // Associate the vertex buffer with the vertex array
    wall_component.vertex_array.Initialise();
    err = wall_component.vertex_array.AssociateVertexBuffer(wall_component.model_data);
  }
  if (err.Good()) {
    // Initialise the shader program with our vert/frag shaders
    err = wall_component.shader_program.Initialise(&vert_shader, &frag_shader);
  }
  wall_component.shader_program.Use();
  wall_component.shader_program.SetInt("Texture1", 0);
  wall_component.texture = tex;

  auto dandan = ecs_->CreateEntity();
  if (dandan && err.Good()) {
    err = dandan->AddComponent(mesh_cid_, wall_component);
    if (err.Good()) {
      evie::TransformComponent transform;
      // NOLINTNEXTLINE(*-union-access)
      transform.position.y = dandan_height_offset;
      transform.scale = dandan_scale;
      err = dandan->AddComponent(transform_cid_, transform);
      if (err.Good()) {
        // Dan dan is a follower
        err = dandan->AddComponent(follower_cid_);
      }
    }
  }

  return err;
}

evie::Error GameLayer::SetupPlayer(float map_scale)
{
  (void)map_scale;
  evie::Error err = evie::Error::OK();

  // Create player entity
  auto player_entity = ecs_->CreateEntity();
  if (player_entity.Good()) {
    player_entity->MoveEntity(player_entity_);
    if (err.Good()) {
      err = player_entity_->AddComponent(transform_cid_);
      if (err.Good()) {
        err = player_entity_->AddComponent(follow_target_cid_);
      }
      if (err.Good()) {
        err = player_entity_->AddComponent(velocity_cid_);
      }
    }
  } else {
    if (err.Good()) {
      err = player_entity.Error();
    }
  }

  // Create projectile system
  // The projectile system will create projectiles entities when a mouse is clicked.
  // The projectile system will destroy the projectile if it comes into contact with a target.
  // When the mouse is clicked the projectile will have a velocity added to it and the physics system.
  // will move anything with velocity.


  return err;
}