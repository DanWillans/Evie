#ifndef EVIE_INCLUDE_RENDERING_SIMPLE_RENDERER_HPP_
#define EVIE_INCLUDE_RENDERING_SIMPLE_RENDERER_HPP_

#include <evie/asset_manager_interface.hpp>
#include <evie/camera.h>
#include <evie/renderer_interface.hpp>
#include <evie/shader_program.h>


namespace evie {
class EVIE_API SimpleRenderer : public IRenderer
{
public:
  explicit SimpleRenderer(FPSCamera& camera, const ShaderProgramAsset& shader_program);
  void DrawModel(ModelAsset& model, const TransformComponent& component) override;
  void DrawModel(ModelAsset& model, const vec3& position, const quat& rotation, const vec3& scale) override;

private:
  FPSCamera& camera_;
  ShaderProgramAsset shader_program_;
};
}// namespace evie

#endif// EVIE_INCLUDE_RENDERING_SIMPLE_RENDERER_HPP_