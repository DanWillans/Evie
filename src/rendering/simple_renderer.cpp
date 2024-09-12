#include "rendering/simple_renderer.hpp"

#include "evie/asset_manager_interface.hpp"
#include "evie/camera.h"
#include "evie/ecs/components/transform.hpp"
#include "evie/model.hpp"

#include <evie/shader_program.h>
#include <evie/window.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/ext/matrix_clip_space.hpp>


namespace evie {

SimpleRenderer::SimpleRenderer(IWindow& window, FPSCamera& camera, const ShaderProgramAsset& shader_program)
  : window_(window), camera_(camera), shader_program_(shader_program)
{}
void SimpleRenderer::DrawModel(ModelAsset& model, const TransformComponent& transform)
{
  DrawModel(model, transform.position, transform.rotation, transform.scale);
}

void SimpleRenderer::DrawModel(ModelAsset& model, const vec3& position, const quat& rotation, const vec3& scale)
{
  shader_program_.Get()->Use();
  auto& model_prog = *shader_program_.Get();

  evie::mat4 model_transform(1.0F);

  // 1. Translate the model
  model_transform = glm::translate(model_transform, position);

  // 2. Rotate the model
  model_transform *= glm::toMat4(rotation);

  // 3. Scale the model
  model_transform = glm::scale(model_transform, scale);

  // 4. Setup lighting
  model_prog.SetVec3("directional_light.direction", { 0.0f, -1.0f, 0.0f });
  model_prog.SetVec3("directional_light.ambient", { 0.5f, 0.5f, 0.5f });
  model_prog.SetVec3("directional_light.diffuse", { 0.5f, 0.5f, 0.5f });
  model_prog.SetVec3("directional_light.specular", { 1.0f, 1.0f, 1.0f });

  // 5. Setup material stuff
  model_prog.SetFloat("material.shininess", 225.0F);

  // 6. Setup model/view/projection
  model_prog.SetMat4("model", glm::value_ptr(model_transform));
  evie::mat4 view = camera_.GetViewMatrix();
  model_prog.SetMat4("view", glm::value_ptr(view));
  view = glm::inverseTranspose(view);
  model_prog.SetMat4("inverse_transpose_view", glm::value_ptr(view));
  // This sets up the projection. What's our FoV? What's our aspect ratio? Fix this to get from camera.
  constexpr float near_cull = 0.1F;
  constexpr float far_cull = 1000.0F;
  evie::mat4 projection =
    glm::perspective(glm::radians(camera_.field_of_view), window_.GetAspectRatio(), near_cull, far_cull);
  model_prog.SetMat4("projection", glm::value_ptr(projection));

  // Setup the shader program and give to the model object to draw.
  if (model.IsValid()) {
    model.Get()->Draw(*shader_program_.Get());
  }
}
}// namespace evie