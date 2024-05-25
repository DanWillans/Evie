#ifndef INCLUDE_RENDER_HPP_
#define INCLUDE_RENDER_HPP_

#include <evie/ecs/components/Transform.hpp>
#include <evie/ecs/components/mesh_component.hpp>
#include <evie/ecs/system.hpp>
#include <evie/ids.h>
#include <evie/camera.h>

class Renderer : public evie::System
{
public:
  void Initialise(evie::ComponentID<evie::MeshComponent> mesh_cid,
    evie::ComponentID<evie::TransformRotationComponent> transform_cid,
    evie::FPSCamera* camera);
  void Iterate();

private:
  evie::ComponentID<evie::MeshComponent> mesh_cid_{ 0 };
  evie::ComponentID<evie::TransformRotationComponent> transform_cid_{ 0 };
  evie::FPSCamera* camera_;
};

#endif// !INCLUDE_RENDER_HPP_