#include "rendering/simple_renderer.hpp"

#include "evie/camera.h"
#include "evie/ecs/components/transform.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


namespace evie {

SimpleRenderer::SimpleRenderer(Camera& camera) : camera_(camera) {}
void SimpleRenderer::DrawModel(ModelAsset& model, const TransformComponent& transform)
{
  DrawModel(model, transform.position, transform.rotation, transform.scale);
}

void SimpleRenderer::DrawModel(ModelAsset& model, const vec3& position, const quat& rotation, const vec3& scale)
{
  evie::mat4 model_transform(1.0F);

  // 1. Translate the model
  model_transform = glm::translate(model_transform, position);

  // 2. Rotate the model
  model_transform *= glm::toMat4(rotation);

  // 3. Scale the model
  model_transform = glm::scale(model_transform, scale);

  // Setup the shader program and give to the model object to draw.
  if (model.IsValid()) {
    model.Get()->Draw(shader_program_);
  }
}
}// namespace evie
