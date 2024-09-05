#ifndef EVIE_INCLUDE_RENDERER_HPP_
#define EVIE_INCLUDE_RENDERER_HPP_

#include "evie/asset_manager_interface.hpp"
#include "evie/ecs/components/transform.hpp"


namespace evie {

class IRenderer
{
public:
  IRenderer() = default;
  IRenderer(const IRenderer&) = delete;
  IRenderer(IRenderer&&) = delete;
  IRenderer& operator=(const IRenderer&) = delete;
  IRenderer& operator=(IRenderer&&) = delete;
  virtual ~IRenderer() = default;
  virtual void DrawModel(ModelAsset& model, const TransformComponent& transform) = 0;
  virtual void DrawModel(ModelAsset& model, const vec3& position, const quat& rotation, const vec3& scale) = 0;
};

}// namespace evie

#endif// !EVIE_INCLUDE_RENDERER_HPP_