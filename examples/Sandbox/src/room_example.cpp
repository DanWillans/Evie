#include <evie/core.h>
#include <evie/ecs/components/Transform.hpp>
#include <evie/ecs/components/mesh_component.hpp>
#include <evie/ecs/components/transform.hpp>
#include <evie/ecs/system_manager.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/matrix.hpp>
#include <memory>
#define _USE_MATH_DEFINES
#include <math.h>

#include "evie/application.h"
#include "evie/camera.h"
#include "evie/default_models.h"
#include "evie/ecs/components/transform.hpp"
#include "evie/ecs/components/velocity.hpp"
#include "evie/ecs/ecs_controller.h"
#include "evie/error.h"
#include "evie/events.h"
#include "evie/evie.h"
#include "evie/ids.h"
#include "evie/indices_array.h"
#include "evie/input.h"
#include "evie/input_manager.h"
#include "evie/key_events.h"
#include "evie/layer.h"
#include "evie/logging.h"
#include "evie/mouse_events.h"
#include "evie/shader.h"
#include "evie/shader_program.h"
#include "evie/texture.h"
#include "evie/types.h"
#include "evie/vertex_array.h"
#include "evie/vertex_buffer.h"

#include "imgui.h"

#include "GLFW/glfw3.h"
namespace {

class PhysicsSystem : public evie::System
{
public:
  PhysicsSystem(evie::ComponentID<evie::TransformRotationComponent> transform_component_id,
    evie::ComponentID<evie::VelocityComponent> velocity_component_id)
    : transform_component_id_(transform_component_id), velocity_component_id_(velocity_component_id)
  {}
  void Update(float delta_time) const
  {
    for (const auto& entity : entities) {
      auto& translate = component_manager->GetComponent(entity, transform_component_id_);
      const auto& velocity = component_manager->GetComponent(entity, velocity_component_id_);
      translate.position.x += delta_time * velocity.velocity.x;
      translate.position.y += delta_time * velocity.velocity.y;
      translate.position.z += delta_time * velocity.velocity.z;
    }
  }

private:
  evie::ComponentID<evie::TransformRotationComponent> transform_component_id_{ 0 };
  evie::ComponentID<evie::VelocityComponent> velocity_component_id_{ 0 };
};

class RenderCubeSystem : public evie::System
{
public:
  RenderCubeSystem(evie::Camera* camera,
    evie::ComponentID<evie::TransformRotationComponent> transform_componend_id,
    evie::ComponentID<evie::MeshComponent> mesh_component_id)
    : camera_(camera), transform_component_id_(transform_componend_id), mesh_component_id_(mesh_component_id)
  {}
  void Update() const
  {
    for (const auto& entity : entities) {
      evie::mat4 model(1.0f);
      const auto& translate = component_manager->GetComponent(entity, transform_component_id_);
      const auto& mesh = component_manager->GetComponent(entity, mesh_component_id_);

      // Handle transforming the object first
      // This moves the object to where we want it in world space.
      model = glm::translate(model, translate.position);
      // This rotates the object to where we want it in the world space.
      auto rot_mat4 = glm::eulerAngleXYZ(translate.rotation.x, translate.rotation.y, translate.rotation.z);
      model = model * rot_mat4;
      model = glm::scale(model, translate.scale);

      // Bind VAO and Shader Program
      auto& shader_program = mesh.shader_program;
      shader_program.Use();
      if (shader_program.HasVec3("viewPos")) {
        shader_program.SetVec3("viewPos", camera_->GetPosition());
      }
      mesh.vertex_array.Bind();

      // Update uniforms in the shader program
      shader_program.SetMat4("model", glm::value_ptr(model));
      evie::mat4 view = camera_->GetViewMatrix();
      shader_program.SetMat4("view", glm::value_ptr(view));
      view = glm::inverseTranspose(view);
      shader_program.SetMat4("inverse_transpose_view", glm::value_ptr(view));
      // This sets up the projection. What's our FoV? What's our aspect ratio? Fix this to get from camera.
      evie::mat4 projection = glm::perspective(glm::radians(camera_->field_of_view), 1920.0f / 1080.0f, 0.1f, 100.0f);
      shader_program.SetMat4("projection", glm::value_ptr(projection));

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
  }


private:
  evie::ComponentID<evie::TransformRotationComponent> transform_component_id_{ 0 };
  evie::ComponentID<evie::MeshComponent> mesh_component_id_{ 0 };
  evie::Camera* camera_;
};
}// namespace

class GameLayer : public evie::Layer
{
public:
  GameLayer() = default;
  explicit GameLayer(const evie::IInputManager* input_manager, evie::IWindow* window, evie::ECSController* ecs)
    : input_manager_(input_manager), window_(window), ecs_(ecs)
  {}

  evie::Error Initialise()
  {
    // ----- Textures -----
    evie::Error err = diffuse_map_.Initialise(
      "C:\\Users\\willa\\devel\\Evie\\out\\install\\windows-msvc-debug-developer-mode\\textures\\container2.png");
    if (err.Good()) {
      err = specular_map_.Initialise(
        "C:\\Users\\willa\\devel\\Evie\\out\\install\\windows-msvc-debug-developer-mode\\textures\\container2_specular."
        "png");
    }
    if (err.Good()) {
      err = emission_map_.Initialise(
        "C:\\Users\\willa\\devel\\Evie\\out\\install\\windows-msvc-debug-developer-mode\\textures\\matrix.jpg");
    }
    evie::Texture2D nice_;
    if (err.Good()) {
      err = nice_.Initialise(
        "C:\\Users\\willa\\devel\\Evie\\out\\install\\windows-msvc-debug-developer-mode\\textures\\nice_scene.jpg",
        true);
    }
    // ----- Shaders -----
    if (err.Good()) {
      err = lighting_vertex_shader_.Initialise("C:\\Users\\willa\\devel\\Evie\\shaders\\light_vert_diffuse_shader.vs");
    }
    if (err.Good()) {
      err =
        lighting_fragment_shader_.Initialise("C:\\Users\\willa\\devel\\Evie\\shaders\\light_frag_diffuse_shader.fs");
    }
    evie::ShaderProgram shader_program;
    if (err.Good()) {
      err = shader_program.Initialise(&lighting_vertex_shader_, &lighting_fragment_shader_);
    }
    // Lightsource shader
    evie::VertexShader lighting_source_vertex_shader;
    if (err.Good()) {
      err =
        lighting_source_vertex_shader.Initialise("C:\\Users\\willa\\devel\\Evie\\shaders\\light_source_vert_shader.vs");
    }
    evie::FragmentShader lighting_source_fragment_shader;
    if (err.Good()) {
      err = lighting_source_fragment_shader.Initialise(
        "C:\\Users\\willa\\devel\\Evie\\shaders\\light_source_frag_shader.fs");
    }
    evie::ShaderProgram light_source_shader_program;
    if (err.Good()) {
      err = light_source_shader_program.Initialise(&lighting_source_vertex_shader, &lighting_source_fragment_shader);
    }

    // ----- Initialise Vertex Objects -----
    // Initialise cube first
    evie::MeshComponent mesh_component;
    mesh_component.shader_program = shader_program;
    evie::VertexBuffer vbuffer;
    if (err.Good()) {
      evie::BufferLayout layout;
      layout.stride = 8;
      layout.type = evie::VertexDataType::Float;
      layout.layout_sizes = { 3, 3, 2 };
      err = vbuffer.Initialise(evie::default_models::cube_with_normals_and_tex_coords, layout);
    }

    if (err.Good()) {
      mesh_component.vertex_array.Initialise();
      err = mesh_component.vertex_array.AssociateVertexBuffer(vbuffer);
    }

    // Initialise light source
    evie::MeshComponent light_source_mesh_component;
    light_source_mesh_component.shader_program = light_source_shader_program;
    light_source_mesh_component.vertex_array.Initialise();
    // light_source_mesh_component.model_data = mesh_component.model_data;
    light_source_mesh_component.vertex_array.AssociateVertexBuffer(vbuffer);

    // Disable cursor initially
    window_->DisableCursor();

    // Register components with ECS
    transform_component_id_ = ecs_->RegisterComponent<evie::TransformRotationComponent>();
    mesh_component_id_ = ecs_->RegisterComponent<evie::MeshComponent>();
    velocity_component_id_ = ecs_->RegisterComponent<evie::VelocityComponent>();

    // Register system with ECS
    evie::SystemSignature render_signature;
    render_signature.SetComponent(transform_component_id_);
    render_signature.SetComponent(mesh_component_id_);
    render_cube_system_id_ =
      ecs_->RegisterSystem<RenderCubeSystem>(render_signature, &camera_, transform_component_id_, mesh_component_id_);
    cube_render_ = ecs_->GetSystem(render_cube_system_id_);

    evie::SystemSignature physics_signature;
    physics_signature.SetComponent(transform_component_id_);
    physics_signature.SetComponent(velocity_component_id_);
    physics_system_id_ =
      ecs_->RegisterSystem<PhysicsSystem>(physics_signature, transform_component_id_, velocity_component_id_);
    physics_system_ = ecs_->GetSystem(physics_system_id_);

    // Create cubes
    for (unsigned int i = 0; i < 10; ++i) {
      auto entity = ecs_->CreateEntity();
      if (entity.Good()) {
        evie::TransformRotationComponent transform;
        transform.rotation = (glm::radians(20.0f * i) * glm::vec3(1.0f, 0.3f, 0.5f));
        transform.position = cubePositions[i];
        transform.scale = { 1.0, 1.0, 1.0 };
        err = entity->AddComponent(transform_component_id_, transform);
      }
      if (err.Good()) {
        // Update the color here
        mesh_component.shader_program.Use();
        mesh_component.shader_program.SetInt("material.diffuse", 0);
        diffuse_map_.SetSlot(0);
        mesh_component.shader_program.SetInt("material.specular", 1);
        specular_map_.SetSlot(1);
        mesh_component.shader_program.SetInt("material.emission", 2);
        emission_map_.SetSlot(2);
        mesh_component.shader_program.SetInt("spot_light.image", 3);
        nice_.SetSlot(3);
        // Directional light (The Sun!)
        mesh_component.shader_program.SetVec3("directional_light.direction", { 0.0f, -1.0f, 0.0f });
        mesh_component.shader_program.SetVec3("directional_light.ambient", { 0.2f, 0.2f, 0.2f });
        mesh_component.shader_program.SetVec3("directional_light.diffuse", { 0.5f, 0.5f, 0.5f });
        mesh_component.shader_program.SetVec3("directional_light.specular", { 1.0f, 1.0f, 1.0f });
        mesh_component.shader_program.SetFloat("material.shininess", 32.0f);
        for (unsigned int i = 0; i < 4; ++i) {
          mesh_component.shader_program.SetVec3(
            "point_lights[" + std::to_string(i) + "].position", { pointLightPositions[i] });
          mesh_component.shader_program.SetFloat("point_lights[" + std::to_string(i) + "].linear", 0.09f);
          mesh_component.shader_program.SetFloat("point_lights[" + std::to_string(i) + "].quadratic", 0.032f);
          mesh_component.shader_program.SetFloat("point_lights[" + std::to_string(i) + "].constant", 1.0f);
          mesh_component.shader_program.SetVec3(
            "point_lights[" + std::to_string(i) + "].ambient", { 0.2f, 0.2f, 0.2f });
          mesh_component.shader_program.SetVec3(
            "point_lights[" + std::to_string(i) + "].diffuse", { 0.5f, 0.5f, 0.5f });
          mesh_component.shader_program.SetVec3(
            "point_lights[" + std::to_string(i) + "].specular", { 1.0f, 1.0f, 1.0f });
        }
        // Spot_light is the flashlight from the camera
        mesh_component.shader_program.SetVec3("spot_light.position", camera_.GetPosition());
        mesh_component.shader_program.SetVec3("spot_light.direction", camera_.GetDirection());
        mesh_component.shader_program.SetFloat("spot_light.cut_off", glm::cos(glm::radians(12.5f)));
        mesh_component.shader_program.SetFloat("spot_light.outer_cut_off", glm::cos(glm::radians(15.0f)));
        mesh_component.shader_program.SetVec3("spot_light.ambient", { 0.2f, 0.2f, 0.2f });
        mesh_component.shader_program.SetVec3("spot_light.diffuse", { 0.8f, 0.8f, 0.8f });
        mesh_component.shader_program.SetVec3("spot_light.specular", { 1.0f, 1.0f, 1.0f });
        mesh_component.shader_program.SetFloat("spot_light.linear", 0.09f);
        mesh_component.shader_program.SetFloat("spot_light.quadratic", 0.032f);
        mesh_component.shader_program.SetFloat("spot_light.constant", 1.0f);
        err = entity->AddComponent(mesh_component_id_, mesh_component);
        cube_entities_.push_back(*entity);
      }
    }

    // Create light source
    for (unsigned int i = 0; i < 4; ++i) {
      auto light_entity = ecs_->CreateEntity();
      if (light_entity) {
        evie::TransformRotationComponent transform;
        transform.rotation = { 0.0, 0.0, 0.0 };
        transform.position = pointLightPositions[i];
        transform.scale = { 0.2, 0.2, 0.2 };
        err = light_entity->AddComponent(transform_component_id_, transform);
      }
      if (err.Good()) {
        err = light_entity->AddComponent(mesh_component_id_, light_source_mesh_component);
        auto& light_mesh = light_entity->GetComponent(mesh_component_id_);
        light_mesh.shader_program.Use();
        glm::vec3 lightColor{ 1.0 };
        light_mesh.shader_program.SetVec3("lightColor", lightColor);
      }
      light_entities_.push_back(*light_entity);
    }
    // Initialise camera speed
    camera_.camera_speed = 10;
    return err;
  }

  void OnRender() override
  {
    // if (cursor_enabled_) {
    //   if (!ImGui::Begin("Lighting Example", nullptr, 0)) {
    //     ImGui::End();
    //   }
    //   const char* combo_preview_value = current_material_index_.first;
    //   if (ImGui::BeginCombo("Cube Material", combo_preview_value, 0)) {
    //     for (const auto& material : material_map) {
    //       const bool is_selected = material.first == current_material_index_.first;
    //       if (ImGui::Selectable(material.first, is_selected)) {
    //         current_material_index_ = material;
    //       }
    //       if (is_selected) {
    //         ImGui::SetItemDefaultFocus();
    //       }
    //     }
    //     ImGui::EndCombo();
    //   }
    //   ImGui::End();
    // }
    cube_render_->Update();
  }

  void OnUpdate() override
  {
    // Calculate some delta times between last OnUpdate call
    float current_frame = static_cast<float>(glfwGetTime());
    float delta_time = current_frame - last_frame_;
    last_frame_ = current_frame;

    physics_system_->Update(delta_time);

    // Move light entity
    // auto& light_transform = light_entity_->GetComponent(transform_component_id_);
    // light_transform.position.x = cos(glfwGetTime());
    // light_transform.position.z = sin(glfwGetTime());
    // glm::vec3 lightColor{ 1.0 };
    // glm::vec3 ambientColor = lightColor * glm::vec3(0.2f);
    // glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);

    // auto& light_mesh = light_entity_->GetComponent(mesh_component_id_);
    // light_mesh.shader_program.Use();
    // light_mesh.shader_program.SetVec3("lightColor", lightColor);

    // Handle camera translation
    if (input_manager_->IsKeyPressed(evie::KeyCode::W)) {
      camera_.MoveForwards(delta_time);
    }
    if (input_manager_->IsKeyPressed(evie::KeyCode::S)) {
      camera_.MoveBackwards(delta_time);
    }
    if (input_manager_->IsKeyPressed(evie::KeyCode::A)) {
      camera_.MoveLeft(delta_time);
    }
    if (input_manager_->IsKeyPressed(evie::KeyCode::D)) {
      camera_.MoveRight(delta_time);
    }
    for (int i = 0; i < 10; ++i) {
      auto& mesh_component = cube_entities_[i].GetComponent(mesh_component_id_);
      mesh_component.shader_program.Use();
      mesh_component.shader_program.SetVec3("spot_light.position", camera_.GetPosition());
      mesh_component.shader_program.SetVec3("spot_light.direction", camera_.GetDirection());
    }
  }

  void OnEvent([[maybe_unused]] evie::Event& event) override
  {
    // Process mouse move events for camera rotation
    if (!cursor_enabled_) {
      if (event.GetEventType() == evie::EventType::MouseMoved) {
        const auto* const mouse_event = event.Cast<evie::MouseMovementEvent>();
        camera_.Rotate(mouse_event->GetMousePosition());
        event.handled = true;
        return;
      }
    }

    // Process mouse scrolled events for camera fov
    if (event.GetEventType() == evie::EventType::MouseScrolled) {
      const auto* const scrolled_event = event.Cast<evie::MouseScrolledEvent>();
      auto& fov = camera_.field_of_view;
      fov -= static_cast<float>(scrolled_event->GetScrollOffset().y_offset);
      if (fov < 1.0f) {
        fov = 1.0f;
      }
      if (fov > 45.0f) {
        fov = 45.0f;
      }
      event.handled = true;
      return;
    }

    // Process j/k for mixer values
    if (event.GetEventType() == evie::EventType::KeyPressed) {
      const auto* const key_event = event.Cast<evie::KeyPressedEvent>();
      if (key_event->IsKeyCode(evie::KeyCode::Escape)) {
        if (cursor_enabled_) {
          window_->DisableCursor();
          cursor_enabled_ = false;
        } else {
          window_->EnableCursor();
          cursor_enabled_ = true;
        }
      } else if (key_event->IsKeyCode(evie::KeyCode::Up)) {
        camera_.camera_speed += 0.05f;
      } else if (key_event->IsKeyCode(evie::KeyCode::Down)) {
        camera_.camera_speed -= 0.05f;
      } else if (key_event->IsKeyCode(evie::KeyCode::R)) {
        camera_.ResetCameraPosition();
      }
    }
  }

  void Shutdown() override
  {
    // Destroy entities
    for (auto& entity : cube_entities_) {
      entity.Destroy();
    }
    for (auto& entity : light_entities_) {
      entity.Destroy();
    }
  }


private:
  evie::Camera camera_;
  evie::VertexShader lighting_vertex_shader_;
  evie::FragmentShader lighting_fragment_shader_;
  evie::VertexArray lighting_vertex_array_;
  float last_frame_ = 0.0f;
  bool cursor_enabled_{ false };
  const evie::IInputManager* input_manager_{ nullptr };
  evie::IWindow* window_{ nullptr };
  evie::ECSController* ecs_{ nullptr };
  std::vector<evie::Entity> cube_entities_;
  std::vector<evie::Entity> light_entities_;
  evie::ComponentID<evie::TransformRotationComponent> transform_component_id_{ 0 };
  evie::ComponentID<evie::VelocityComponent> velocity_component_id_{ 0 };
  evie::ComponentID<evie::MeshComponent> mesh_component_id_{ 0 };
  evie::SystemID<RenderCubeSystem> render_cube_system_id_{ 0 };
  evie::SystemID<PhysicsSystem> physics_system_id_{ 0 };
  const RenderCubeSystem* cube_render_{ nullptr };
  const PhysicsSystem* physics_system_{ nullptr };
  evie::Entity* light_entity_;
  evie::Entity* cube_entity_;
  std::pair<const char*, Material> current_material_index_{ *material_map.begin() };
  evie::Texture2D diffuse_map_;
  evie::Texture2D specular_map_;
  evie::Texture2D emission_map_;
  evie::Texture2D nice_;
};


class Sandbox : public evie::Application
{
public:
  [[nodiscard]] evie::Error Init()
  {
    evie::WindowProperties props;
    props.name = "Sandbox";
    props.dimensions.width = 1920;
    props.dimensions.height = 1080;
    // Always Initialise the engine before doing anything with it.
    APP_INFO("Initialiasing engine");
    evie::Error err = Initialise(props);
    // Set ImGui context from the engine.
    ImGui::SetCurrentContext(GetImGuiContext());
    if (err.Good()) {
      APP_INFO("Creating GameLayer");
      t_layer_ = GameLayer(GetInputManager(), GetWindow(), GetECSController());
      APP_INFO("Initialising layer");
      err = t_layer_.Initialise();
      if (err.Good()) {
        APP_INFO("Pushing layer");
        PushLayerFront(t_layer_);
      }
    }
    return err;
  }
  ~Sandbox() override = default;

private:
  GameLayer t_layer_;
};

std::unique_ptr<evie::Application> CreateApplication()
{
  APP_INFO("Making Sandbox");
  std::unique_ptr<Sandbox> app = std::make_unique<Sandbox>();
  APP_INFO("Initialising Sandbox");
  evie::Error err = app->Init();
  if (err.Good()) {
    APP_INFO("Sandbox Initialised Succesfully");
    return app;
  } else {
    APP_INFO("App failed to initialise. Reason: {}", err.Message());
    return nullptr;
  }
}