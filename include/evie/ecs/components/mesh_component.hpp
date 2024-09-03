#ifndef EVIE_INCLUDE_EVIE_ECS_COMPONENTS_MESH_COMPONENT_HPP_
#define EVIE_INCLUDE_EVIE_ECS_COMPONENTS_MESH_COMPONENT_HPP_

#include "evie/asset_manager_interface.hpp"
#include "evie/shader_program.h"
#include "evie/vertex_array.h"
#include "evie/vertex_buffer.h"

namespace evie {
struct MeshComponent
{
  VertexBuffer<> model_data;
  VertexArray<> vertex_array;
  ShaderProgram shader_program;
  Texture2DAsset texture;
  // How do we handle cleaning up these resources?

  int GetModelIndices() const { return model_data.GetBuffer().size() / model_data.GetBufferLayout().stride; }
};

}// namespace evie
#endif// !EVIE_INCLUDE_EVIE_ECS_COMPONENTS_MESH_COMPONENT_HPP_
