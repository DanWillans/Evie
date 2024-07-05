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
  vec3 position;
  vec3 normal;
  vec2 tex_coords;
};

template<typename VertexType = Vertex> class Mesh
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

  Error Initialise() { SetupMesh(); }

  void Draw(ShaderProgram& shader);

private:
  VertexArray<VertexType> vertex_array_;
  VertexBuffer<VertexType> vertex_buffer_;
  IndicesArray indices_array_;
  Error SetupMesh();
};

template<typename VertexType> void Mesh<VertexType>::Draw(ShaderProgram& shader_program)
{
  int diffuseNr = 1;
  int specularNr = 1;
  for (int i = 0; i < textures.size(); i++) {
    textures[i].SetSlot(i);
    std::string texture_name;
    TextureType& type = textures[i].type;
    if (type == TextureType::Diffuse) {
      texture_name = "texture_diffuse" + std::to_string(diffuseNr++);
    } else if (type == TextureType::Specular) {
      texture_name = "texture_specular" + std::to_string(specularNr++);
    }
    shader_program.SetInt(("material." + texture_name).c_str(), i);
    textures[i].Bind();
  }
  // Reset current active texture
  if (!textures.empty()) {
    textures[0].SetSlot(0);
  }

  // Draw mesh
  vertex_array_.Bind();
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

template<typename VertexType> Error Mesh<VertexType>::SetupMesh()
{
  evie::Error err = Error::OK();
  vertex_array_.Initialise();

  const BufferLayout mesh_layout{
    sizeof(Vertex),
    // Be careful here with VertexDataType::Float if Vertex has anything other than a float type the
    // AssociateVertexBuffer will fail. This will need fixing if we change the Vertex struct in the future.
    VertexDataType::Float,
    { sizeof(Vertex::position), sizeof(Vertex::normal), sizeof(Vertex::tex_coords) },
  };

  if (err.Good()) {
    err = vertex_buffer_.Initialise(vertices, mesh_layout);
  }

  if (err.Good()) {
    indices_array_.Initialise(indices);
  }

  if (err.Good()) {
    err = vertex_array_.AssociateVertexBuffer(vertex_array_);
  }

  if (err.Good()) {
    err = vertex_array_.AssociateIndicesArray(indices_array_);
  }
}

}// namespace evie

#endif// !EVIE_INCLUDE_RENDERING_MESH_HPP_