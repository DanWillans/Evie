
#include "rendering/model.hpp"
#include "evie/shader_program.h"

#include <assimp/Importer.hpp>


namespace evie {

void Model::LoadModel(const std::string& path) {
  Assimp::Importer importer;
}

void Model::Draw(ShaderProgram& shader_program)
{
  for (auto i = 0; i < meshes.size(); ++i) {}
}
}// namespace evie