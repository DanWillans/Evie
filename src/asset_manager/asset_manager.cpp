#include <cstdlib>
#include <filesystem>
#include <memory>
#include <utility>

#include "asset_manager/asset_manager.hpp"

#include "evie/asset_manager_interface.hpp"
#include "evie/error.h"
#include "evie/logging.h"
#include "evie/result.h"
#include "evie/shader_program.h"
#include <evie/default_models.h>
#include <evie/mesh.hpp>

#include "whereami/whereami.h"

namespace evie {
AssetManager::AssetManager()
{
  int length{ 0 };
  int dirname_length{ 0 };

  length = wai_getExecutablePath(nullptr, 0, &dirname_length);
  if (length > 0) {
    std::vector<char> path;
    path.resize(length + 1);
    wai_getExecutablePath(path.data(), length, &dirname_length);
    path[length] = '\0';
    path[dirname_length] = '\0';
    asset_directory_ = std::filesystem::path(std::string(path.data()) + "/../assets");
  } else {
    EV_ERROR("AssetManager failed");
    assertm(false, "AssetManager failed");
  }
}

Result<Texture2DAsset> AssetManager::GetTexture2D(const std::string& texture_name, TextureWrapping texture_wrapping)
{
  size_t hash = std::hash<std::string>{}(texture_name);
  if (auto it = texture_2d_map_.find(hash); it != texture_2d_map_.end()) {
    // Return a new AssetProxy. This doesn't reload the data just constructs a new proxy with a reference to the
    // already loaded assets, which in turn will increase the reference count on this asset.
    EV_DEBUG("Texture \"{}\" exists. Reusing", texture_name);
    Texture2D& texture_2d = it->second.asset;
    return Texture2DAsset{ this, { AssetType::Texture2D, hash }, &texture_2d };
  } else {
    // Read texture from file system and insert into texture_2d_map_.
    // Check if this file exists or not.
    std::filesystem::path asset_path = asset_directory_;
    asset_path /= "textures";
    asset_path /= texture_name;
    if (std::filesystem::exists(asset_path)) {
      EV_DEBUG("Creating texture asset from path {}", asset_path.string());
      Texture2D texture;
      texture.Initialise(asset_path.string(), true, texture_wrapping);
      // Add to map
      auto texture_2d_asset = texture_2d_map_.emplace(hash, texture);
      Texture2D& texture_2d = texture_2d_asset.first->second.asset;
      return Texture2DAsset{ this, { AssetType::Texture2D, hash }, &texture_2d };
    } else {
      EV_WARN("Texture {} doesn't exist. Using default engine texture.", texture_name);
      return Error{ "Texture doesn't exist for AssetManager to Load" };
    }
  }
}

Result<ShaderProgramAsset> AssetManager::GetShaderProgram(const std::string& shader_name)
{
  Error err = Error::OK();
  size_t hash = std::hash<std::string>{}(shader_name);
  if (auto it = shader_program_map_.find(hash); it != shader_program_map_.end()) {
    EV_DEBUG("ShaderProgram \"{}\" exists. Reusing", shader_name);
    ShaderProgram& shader_program = it->second.asset;
    return ShaderProgramAsset{ this, { AssetType::ShaderProgram, hash }, &shader_program };
  } else {
    std::filesystem::path asset_path = asset_directory_;
    asset_path /= "shaders";
    asset_path /= shader_name;

    // Load fragment shader
    std::filesystem::path fragment_path = asset_path;
    fragment_path += ".fs";
    FragmentShader frag_shader;
    if (std::filesystem::exists(fragment_path)) {
      err = frag_shader.Initialise(fragment_path.string());
    } else {
      EV_WARN("Shader {} doesn't exist.", shader_name);
      return Error{ "Fragment shader doesn't exist" };
    }

    // Load vertex shader
    std::filesystem::path vertex_path = asset_path;
    vertex_path += ".vs";
    VertexShader vert_shader;
    if (std::filesystem::exists(vertex_path) && err.Good()) {
      err = vert_shader.Initialise(vertex_path.string());
    } else {
      EV_WARN("Shader {} doesn't exist.", shader_name);
      return Error{ "Vertex shader doesn't exist" };
    }

    // Compile the shader program
    if (err.Good()) {
      ShaderProgram shader_program;
      err = shader_program.Initialise(&vert_shader, &frag_shader);
      if (err.Good()) {
        EV_DEBUG("Creating shader asset from vertex path {} and fragment path {}",
          vertex_path.string(),
          fragment_path.string());
        auto shader_program_asset = shader_program_map_.emplace(hash, shader_program);
        ShaderProgram& shader_prog = shader_program_asset.first->second.asset;
        return ShaderProgramAsset{ this, { AssetType::ShaderProgram, hash }, &shader_prog };
      } else {
        return Error{ "Shader program failed to initialise" };
      }
    }

    return err;
  }
}

Result<ModelAsset> AssetManager::GetModel(const std::string& model_name)
{
  size_t hash = std::hash<std::string>{}(model_name);
  if (auto it = model_map_.find(hash); it != model_map_.end()) {
    // Return a new AssetProxy. This doesn't reload the data just constructs a new proxy with a reference to the
    // already loaded assets, which in turn will increase the reference count on this asset.
    EV_DEBUG("Model \"{}\" exists. Reusing", model_name);
    Model& model = it->second.asset;
    return ModelAsset{ shared_from_this(), { AssetType::Model, hash }, &model };
  } else {
    // Read texture from file system and insert into model_map_.
    // Check if this file exists or not.
    std::filesystem::path asset_path = asset_directory_;
    asset_path /= "models";
    asset_path /= model_name;
    asset_path /= model_name;
    asset_path += ".obj";
    if (std::filesystem::exists(asset_path)) {
      EV_DEBUG("Creating model asset from path {}", asset_path.string());
      Model model;
      Error error = model.Initialise(asset_path.string());
      // Add to map
      if (error.Good()) {
        auto model_it = model_map_.emplace(hash, model);
        Model& model_ref = model_it.first->second.asset;
        return ModelAsset{ shared_from_this(), { AssetType::Model, hash }, &model_ref };
      } else {
        return error;
      }
    } else {
      EV_WARN("Model {} doesn't exist.", model_name);
      return Error{ "Model doesn't exist for AssetManager to Load" };
    }
  }
}

Result<ModelAsset> AssetManager::GetModel(default_models::PrimitiveModel model_type,
  const std::vector<Texture2DAsset>& textures)
{
  uint16_t model_num = static_cast<uint16_t>(model_type);
  if (auto it = primitive_model_map_.find(model_num); it != primitive_model_map_.end()) {
    // Return a new AssetProxy. This doesn't reload the data just constructs a new proxy with a reference to the
    // already loaded assets, which in turn will increase the reference count on this asset.
    EV_DEBUG("Model type \"{}\" exists. Reusing", model_num);
    Model& model = it->second.asset;
    return ModelAsset{ shared_from_this(), { AssetType::PrimitiveModel, model_num }, &model };
  } else {
    // We have these types built in hence primitive models. No need to load from file.
    std::vector<Mesh> model_meshes;
    switch (model_type) {
    case default_models::PrimitiveModel::cube:
      model_meshes.emplace_back(default_models::cube_with_pos_norm_tex_indices, default_models::cube_indices, textures);
      model_meshes.back().Initialise();
      break;
    case default_models::PrimitiveModel::cube_upwards:
      model_meshes.emplace_back(
        default_models::cube_with_pos_norm_tex_indices_upwards, default_models::cube_indices, textures);
      model_meshes.back().Initialise();
      break;
    default:
      EV_ERROR("Unknown primitive model type");
      break;
    }
    Model model;
    model.Initialise(model_meshes);
    auto model_it = primitive_model_map_.emplace(model_num, model);
    Model& model_ref = model_it.first->second.asset;
    return ModelAsset{ shared_from_this(), { AssetType::PrimitiveModel, model_num }, &model_ref };
  }
}

void AssetManager::IncreaseReference(AssetMetadata metadata)
{
  switch (metadata.type) {
  case AssetType::Texture2D: {
    texture_2d_map_.at(metadata.hash).reference_count++;
    break;
  }
  case AssetType::ShaderProgram:
    shader_program_map_.at(metadata.hash).reference_count++;
    break;
  case AssetType::Model:
    model_map_.at(metadata.hash).reference_count++;
    break;
  case AssetType::PrimitiveModel:
    primitive_model_map_.at(metadata.hash).reference_count++;
    break;
  default:
    EV_ERROR("Unknown asset type %d", static_cast<uint16_t>(metadata.type));
  }
}

void AssetManager::DecreaseReference(AssetMetadata metadata)
{
  switch (metadata.type) {
  case AssetType::Texture2D: {
    auto& asset_handle = texture_2d_map_.at(metadata.hash);
    asset_handle.reference_count--;
    if (asset_handle.reference_count == 0) {
      asset_handle.asset.Destroy();
      // Reference above is invalid after this erase. DO NOT USE IT anymore.
      texture_2d_map_.erase(metadata.hash);
    }
    break;
  }
  case AssetType::ShaderProgram: {
    auto& asset_handle = shader_program_map_.at(metadata.hash);
    asset_handle.reference_count--;
    if (asset_handle.reference_count == 0) {
      asset_handle.asset.Destroy();
      // Reference above is invalid after this erase. DO NOT USE IT anymore.
      shader_program_map_.erase(metadata.hash);
    }
    break;
  }
  case AssetType::Model: {
    auto& asset_handle = model_map_.at(metadata.hash);
    asset_handle.reference_count--;
    if (asset_handle.reference_count == 0) {
      asset_handle.asset.Destroy();
      // Reference above is invalid after this erase. DO NOT USE IT anymore.
      shader_program_map_.erase(metadata.hash);
    }
    break;
  }
  case AssetType::PrimitiveModel: {
    auto& asset_handle = primitive_model_map_.at(metadata.hash);
    asset_handle.reference_count--;
    if (asset_handle.reference_count == 0) {
      asset_handle.asset.Destroy();
      // Reference above is invalid after this erase. DO NOT USE IT anymore.
      primitive_model_map_.erase(metadata.hash);
    }
    break;
  }
  default:
    EV_ERROR("Unknown asset type %d", static_cast<uint16_t>(metadata.type));
    break;
  }
}

}// namespace evie