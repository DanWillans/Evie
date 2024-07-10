#ifndef EVIE_INCLUDE_RENDERING_MODEL_HPP_
#define EVIE_INCLUDE_RENDERING_MODEL_HPP_

#include <evie/texture.h>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <evie/shader_program.h>

#include <rendering/mesh.hpp>

namespace evie {
class Model
{
public:
  Model(const std::string& path) : path_(path) {}
  Error EVIE_API Initialise();
  void EVIE_API Draw(ShaderProgram& shader);

private:
  // model data
  std::vector<Mesh<>> meshes_;
  std::string directory_;
  std::string path_;
  std::unordered_map<std::string, Texture2D> loaded_textures_;

  Error LoadModel(const std::string& path);
  void ProcessNode(aiNode* node, const aiScene* scene);
  Mesh<> ProcessMesh(aiMesh* mesh, const aiScene* scene);
  std::vector<Texture2D> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);
};
}// namespace evie

#endif// !EVIE_INCLUDE_RENDERING_MODEL_HPP_
