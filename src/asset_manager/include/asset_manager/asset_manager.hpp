#ifndef EVIE_ASSET_MANAGER_INCLUDE_ASSET_MANAGER_HPP_
#define EVIE_ASSET_MANAGER_INCLUDE_ASSET_MANAGER_HPP_

#include <atomic>
#include <filesystem>
#include <string>
#include <unordered_map>

#include "asset_manager_interface.hpp"
#include "asset_types.hpp"

#include "evie/logging.h"
#include "evie/texture.h"

namespace evie {

template<typename AssetType> class AssetProxy
{
public:
  AssetProxy() = delete;

private:
  friend class AssetManager;
  AssetProxy(IAssetManager* asset_manager, AssetMetadata metadata, AssetType* asset)
    : asset_manager_(asset_manager), metadata_(metadata), asset_(asset)
  {
    asset_manager_->IncreaseReference(metadata_);
  }
  ~AssetProxy() { asset_manager_->DecreaseReference(metadata_); }
  AssetType* asset_;
  AssetMetadata metadata_;
  IAssetManager* asset_manager_;
};

using Texture2DAsset = AssetProxy<Texture2D>;

class EVIE_API AssetManager : public IAssetManager
{
public:
  static constexpr const char* TexturesDirectory = "textures";
  AssetManager();
  AssetManager(const AssetManager&) = delete;
  AssetManager(AssetManager&&) = delete;
  AssetManager& operator=(const AssetManager&) = delete;
  AssetManager& operator=(AssetManager&&) = delete;
  virtual ~AssetManager() = default;

  Texture2DAsset GetTexture2D(const std::string& texture);

private:
  // Friend all AssetProxy types
  template<typename AssetType> friend class AssetProxy;

  // A helper struct to encapsulate the asset alongside a reference count.
  template<typename Asset> struct AssetHandle
  {
    // The actual asset data
    Asset asset;
    // The reference count to see the usage of this asset.
    std::atomic<int> reference_count{ 0 };
  };
  void IncreaseReference(AssetMetadata metadata) override;
  void DecreaseReference(AssetMetadata metadata) override;
  std::unordered_map<size_t, AssetHandle<Texture2D>> texture_2d_map_;
};

}// namespace evie


#endif// !EVIE_ASSET_MANAGER_INCLUDE_ASSET_MANAGER_HPP_