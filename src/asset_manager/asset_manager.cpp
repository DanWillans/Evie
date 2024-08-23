#include <filesystem>

#include "asset_manager/asset_manager.hpp"

#include "evie/asset_manager_interface.hpp"
#include "evie/logging.h"
#include "evie/result.h"

#include "whereami/whereami.h"

namespace evie {
AssetManager::AssetManager()
{
  char* path = nullptr;
  int length;
  int dirname_length;

  length = wai_getExecutablePath(NULL, 0, &dirname_length);
  if (length > 0) {
    path = (char*)malloc(length + 1);
    if (!path)
      abort();
    wai_getExecutablePath(path, length, &dirname_length);
    path[length] = '\0';
    path[dirname_length] = '\0';
    asset_directory_ = std::filesystem::path(std::string(path) + "/../assets");
    free(path);
  } else {
    EV_ERROR("AssetManager failed");
    assertm(false, "AssetManager failed");
  }
}

Texture2DAsset AssetManager::GetTexture2D(const std::string& texture_name)
{
  size_t hash = std::hash<std::string>{}(texture_name);
  if (auto it = texture_2d_map_.find(hash); it != texture_2d_map_.end()) {
    // Return a new AssetProxy. This doesn't reload the data just constructs a new proxy with a reference to the
    // already loaded assets, which in turn will increase the reference count on this asset.
    Texture2D& texture_2d = it->second.asset;
    EV_INFO("Exists, returning existing asset");
    return Texture2DAsset{ this, { AssetType::Texture2D, hash }, &texture_2d };
  } else {
    // Read texture from file system and insert into texture_2d_map_.
    // Check if this file exists or not.
    std::filesystem::path asset_path = asset_directory_ /= "textures";
    asset_path /= texture_name;
    if (std::filesystem::exists(asset_path)) {
      // File exists. Let's create our texture.
      EV_INFO("Doesn't exist, creating asset");
      Texture2D texture;
      texture.Initialise(asset_path.string());
      // Add to map
      auto texture_2d_asset = texture_2d_map_.try_emplace(hash, texture);
      Texture2D& texture_2d = texture_2d_asset.first->second.asset;
      return Texture2DAsset{ this, { AssetType::Texture2D, hash }, &texture_2d };
    } else {
      EV_WARN("Texture {} doesn't exist. Using default engine texture.", texture_name);
      EV_ERROR("AssetManager failed");
      assertm(false, "AssetManager failed");
    }
  }
}

void AssetManager::IncreaseReference(AssetMetadata metadata)
{
  switch (metadata.type) {
  case AssetType::Texture2D:
    EV_INFO("Increasing reference count");
    texture_2d_map_[metadata.hash].reference_count++;
    break;
  default:
    EV_ERROR("Unknown asset type %d", static_cast<uint16_t>(metadata.type));
  }
}

void AssetManager::DecreaseReference(AssetMetadata metadata)
{
  switch (metadata.type) {
  case AssetType::Texture2D:
    auto& asset = texture_2d_map_[metadata.hash];
    EV_INFO("Decreasing reference count");
    asset.reference_count--;
    if (asset.reference_count == 0) {
      EV_INFO("Erasing texture");
      // Reference above is invalid after this erase. DO NOT USE IT anymore.
      texture_2d_map_.erase(metadata.hash);
    }
  }
}

}// namespace evie