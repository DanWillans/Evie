#ifndef EVIE_INCLUDE_RENDERING_MESH_HPP_
#define EVIE_INCLUDE_RENDERING_MESH_HPP_

#include <string>
#include <vector>

#include <evie/ids.h>
#include <evie/indices_array.h>
#include <evie/shader_program.h>
#include <evie/texture.h>
#include <evie/types.h>
#include <evie/vertex_array.h>
#include <evie/vertex_buffer.h>


namespace evie {

struct Vertex
{
  vec3 position{};
  vec3 normal{};
  vec2 tex_coords{};
};

class EVIE_API Mesh
{
public:
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture2D> textures;

  Mesh(const std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices,
    const std::vector<Texture2D>& textures)
    : vertices(vertices), indices(indices), textures(textures)
  {}

  Error Initialise() { return SetupMesh(); }

  void Draw(ShaderProgram& shader);

  void Destroy();

private:
  VertexArray<Vertex> vertex_array_;
  VertexBuffer<Vertex> vertex_buffer_;
  IndicesArray indices_array_;
  Error SetupMesh();
};
}// namespace evie

#endif// !EVIE_INCLUDE_RENDERING_MESH_HPP_