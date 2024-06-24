#include "render.hpp"
#include <evie/ecs/components/Transform.hpp>
#include <evie/ids.h>
#include <evie/window.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

void Renderer::Initialise(evie::ComponentID<evie::MeshComponent> mesh_cid,
  evie::ComponentID<evie::TransformComponent> transform_cid,
  evie::FPSCamera* camera,
  evie::IWindow* window)
{
  mesh_cid_ = mesh_cid;
  transform_cid_ = transform_cid;
  camera_ = camera;
  window_ = window;
}

void Renderer::Update(const float& delta_time)
{
  std::ignore = delta_time;
  // APP_INFO("Render entity count: {}", entities.size());
  for (const auto& entity : entities) {
    evie::mat4 model(1.0F);
    const auto& translate = entity.GetComponent(transform_cid_);
    auto& mesh = entity.GetComponent(mesh_cid_);

    // Handle transforming the object first
    // This moves the object to where we want it in world space.
    model = glm::translate(model, translate.position);
    // This rotates the object to where we want it in the world space.
    model = model * glm::toMat4(translate.rotation);
    model = glm::scale(model, translate.scale);

    // Bind VAO and Shader Program
    auto& shader_program = mesh.shader_program;
    shader_program.Use();
    if (shader_program.HasVec3("viewPos")) {
      shader_program.SetVec3("viewPos", camera_->GetPosition());
    }
    mesh.vertex_array.Bind();

    // Set texture. Only one texture per mesh component atm.
    mesh.texture.SetSlot(0);

    // Update uniforms in the shader program
    shader_program.SetMat4("model", glm::value_ptr(model));
    evie::mat4 view = camera_->GetViewMatrix();
    shader_program.SetMat4("view", glm::value_ptr(view));
    // view = glm::inverseTranspose(view);
    // shader_program.SetMat4("inverse_transpose_view", glm::value_ptr(view));
    // This sets up the projection. What's our FoV? What's our aspect ratio? Fix this to get from camera.
    constexpr float near_cull = 0.1F;
    constexpr float far_cull = 1000.0F;
    evie::mat4 projection =
      glm::perspective(glm::radians(camera_->field_of_view), window_->GetAspectRatio(), near_cull, far_cull);
    shader_program.SetMat4("projection", glm::value_ptr(projection));

    glDrawArrays(GL_TRIANGLES, 0, mesh.GetModelIndices());
  }
}