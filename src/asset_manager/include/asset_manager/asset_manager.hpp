#ifndef EVIE_ASSET_MANAGER_INCLUDE_ASSET_MANAGER_HPP_
#define EVIE_ASSET_MANAGER_INCLUDE_ASSET_MANAGER_HPP_

#include <atomic>
#include <filesystem>
#include <string>
#include <unordered_map>

#include "evie/asset_manager_interface.hpp"
#include "evie/core.h"
#include "evie/shader_program.h"
#include "evie/texture.h"
#include "evie/model.hpp"

namespace evie {

class EVIE_API AssetManager final : public IAssetManager
{
public:
  static constexpr const char* TexturesDirectory = "textures";
  AssetManager();
  AssetManager(const AssetManager&) = delete;
  AssetManager(AssetManager&&) = delete;
  AssetManager& operator=(const AssetManager&) = delete;
  AssetManager& operator=(AssetManager&&) = delete;
  ~AssetManager() override = default;

  Result<Texture2DAsset> GetTexture2D(const std::string& texture,
    TextureWrapping texture_wrapping = TextureWrapping::Repeat) override;

  Result<ShaderProgramAsset> GetShaderProgram(const std::string& shader_name) override;

  Result<ModelAsset> GetModel(const std::string& model_name) override;

  Result<ModelAsset> GetModel(default_models::PrimitiveModel model_type, const std::vector<Texture2DAsset>& textures) override;

private:
  // Friend all AssetProxy types
  template<typename AssetType> friend class AssetProxy;

  // A helper struct to encapsulate the asset alongside a reference count.
  template<typename Asset> struct AssetHandle
  {
    explicit AssetHandle(const Asset& asset_in) : asset(asset_in) {}
    // The actual asset data
    Asset asset;
    // The reference count to see the usage of this asset.
    std::atomic<int> reference_count{ 0 };
  };

  void IncreaseReference(AssetMetadata metadata) override;
  void DecreaseReference(AssetMetadata metadata) override;

  std::unordered_map<size_t, AssetHandle<Texture2D>> texture_2d_map_;
  std::unordered_map<size_t, AssetHandle<ShaderProgram>> shader_program_map_;
  std::unordered_map<size_t, AssetHandle<Model>> model_map_;
  std::unordered_map<uint16_t, AssetHandle<Model>> primitive_model_map_;
  std::filesystem::path asset_directory_;
};

}// namespace evie


#endif// !EVIE_ASSET_MANAGER_INCLUDE_ASSET_MANAGER_HPP_