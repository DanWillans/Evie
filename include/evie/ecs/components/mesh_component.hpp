#ifndef EVIE_INCLUDE_EVIE_ECS_COMPONENTS_MESH_COMPONENT_HPP_
#define EVIE_INCLUDE_EVIE_ECS_COMPONENTS_MESH_COMPONENT_HPP_

#include "evie/types.h"
#include "evie/shader.h"
#include "evie/shader_program.h"
#include "evie/vertex_buffer.h"
#include "evie/vertex_array.h"

namespace evie {
struct MeshComponent
{
  VertexBuffer model_data;
  VertexArray vertex_array;
  ShaderProgram shader_program;
  // How do we handle cleaning up these resources?
};
}// namespace evie
#endif// !EVIE_INCLUDE_EVIE_ECS_COMPONENTS_MESH_COMPONENT_HPP_
