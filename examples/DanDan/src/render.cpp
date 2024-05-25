#include "render.hpp"
#include <evie/ecs/components/Transform.hpp>
#include <evie/ids.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

void Renderer::Initialise(evie::ComponentID<evie::MeshComponent> mesh_cid,
  evie::ComponentID<evie::TransformRotationComponent> transform_cid,
  evie::Camera* camera)
{
  mesh_cid_ = mesh_cid;
  transform_cid_ = transform_cid;
  camera_ = camera;
}

void Renderer::Iterate()
{
  for (const auto& entity : entities) {
    evie::mat4 model(1.0f);
    const auto& translate = entity.GetComponent(transform_cid_);
    const auto& mesh = entity.GetComponent(mesh_cid_);

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
    // view = glm::inverseTranspose(view);
    // shader_program.SetMat4("inverse_transpose_view", glm::value_ptr(view));
    // This sets up the projection. What's our FoV? What's our aspect ratio? Fix this to get from camera.
    evie::mat4 projection = glm::perspective(glm::radians(camera_->field_of_view), 1920.0f / 1080.0f, 0.1f, 100.0f);
    shader_program.SetMat4("projection", glm::value_ptr(projection));

    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
}