#ifndef INCLUDE_RENDER_HPP_
#define INCLUDE_RENDER_HPP_

#include <evie/camera.h>
#include <evie/ecs/components/mesh_component.hpp>
#include <evie/ecs/system.hpp>
#include <evie/ids.h>
#include <evie/window.h>

class Renderer : public evie::System
{
public:
  Renderer() = default;
  Renderer(Renderer&&) = default;
  Renderer(const Renderer&) = default;
  Renderer& operator=(Renderer&&) = default;
  Renderer& operator=(const Renderer&) = default;
  virtual ~Renderer() = default;

  void Initialise(evie::ComponentID<evie::MeshComponent> mesh_cid,
    evie::ComponentID<evie::TransformComponent> transform_cid,
    evie::FPSCamera* camera,
    evie::IWindow* window);

private:
  void Update(const float& delta_time) override;

  evie::ComponentID<evie::MeshComponent> mesh_cid_{ 0 };
  evie::ComponentID<evie::TransformComponent> transform_cid_{ 0 };
  evie::FPSCamera* camera_{ nullptr };
  evie::IWindow* window_{ nullptr };
};

#endif// !INCLUDE_RENDER_HPP_