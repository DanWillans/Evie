#ifndef EVIE_INCLUDE_RENDERING_SIMPLE_RENDERER_HPP_
#define EVIE_INCLUDE_RENDERING_SIMPLE_RENDERER_HPP_

#include <evie/asset_manager_interface.hpp>
#include <evie/camera.h>
#include <evie/renderer_interface.hpp>
#include <evie/shader_program.h>


namespace evie {
class SimpleRenderer : public IRenderer
{
public:
  explicit SimpleRenderer(Camera& camera);
  void DrawModel(ModelAsset& model, const TransformComponent& component) override;
  void DrawModel(ModelAsset& model, const vec3& position, const quat& rotation, const vec3& scale) override;

private:
  Camera& camera_;
  ShaderProgram shader_program_;
};
}// namespace evie

#endif// EVIE_INCLUDE_RENDERING_SIMPLE_RENDERER_HPP_