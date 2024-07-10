
#include "rendering/model.hpp"
#include "evie/error.h"
#include "evie/shader_program.h"
#include "stb/stb_image.h"

#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/types.h>
#include <evie/logging.h>
#include <evie/texture.h>


namespace evie {

Error Model::Initialise() { return LoadModel(path_); }

Error Model::LoadModel(const std::string& path)
{
  Assimp::Importer import;
  const aiScene* scene = nullptr;
  scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

  if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    return Error{ "ASSIMP failed to read file" };
  }

  directory_ = path.substr(0, path.find_last_of('\\'));

  ProcessNode(scene->mRootNode, scene);
  return Error::OK();
}

void Model::Draw(ShaderProgram& shader_program)
{
  for (auto i = 0; i < meshes_.size(); ++i) {
    meshes_[i].Draw(shader_program);
  }
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
  // process all the node's meshes (if any)
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    meshes_.push_back(ProcessMesh(mesh, scene));
    meshes_.back().Initialise();
  }
  // then do the same for each of its children
  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    ProcessNode(node->mChildren[i], scene);
  }
}

Mesh<> Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture2D> textures;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    // process vertex positions, normals and texture coordinates
    vertex.position = vec3{ mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
    vertex.normal = vec3{ mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
    if (mesh->HasTextureCoords(0)) {
      vertex.tex_coords.x = mesh->mTextureCoords[0][i].x;
      vertex.tex_coords.y = mesh->mTextureCoords[0][i].y;
    }
    vertices.push_back(vertex);
  }

  // process indices
  for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; ++j) {
      indices.push_back(face.mIndices[j]);
    }
  }

  // process material
  if (mesh->mMaterialIndex >= 0) {
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    float shininess{ 0.0F };
    aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess);
    std::vector<Texture2D> diffuse_maps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
    std::vector<Texture2D> specular_maps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
  }

  return { vertices, indices, textures };
}

std::vector<Texture2D> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName)
{
  std::vector<Texture2D> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
    aiString texture_name;
    mat->GetTexture(type, i, &texture_name);
    if (auto it = loaded_textures_.find(texture_name.C_Str()); it != loaded_textures_.end()) {
      textures.push_back(it->second);
      continue;
    }
    Texture2D texture;
    std::string texture_path = directory_ + "\\" + texture_name.C_Str();
    Error err = texture.Initialise(texture_path, true);
    if (err.Bad()) {
      EV_ERROR("Texture failed to initialise!");
    }
    if (type == aiTextureType_DIFFUSE) {
      texture.type = TextureType::Diffuse;
    } else if (type == aiTextureType_SPECULAR) {
      texture.type = TextureType::Specular;
    }
    loaded_textures_.emplace(texture_name.C_Str(), texture);
    textures.push_back(texture);
  }
  return textures;
}
}// namespace evie