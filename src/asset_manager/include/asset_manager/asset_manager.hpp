#ifndef EVIE_ASSET_MANAGER_INCLUDE_ASSET_MANAGER_HPP_
#define EVIE_ASSET_MANAGER_INCLUDE_ASSET_MANAGER_HPP_

#include <atomic>
#include <filesystem>
#include <string>
#include <unordered_map>

#include "evie/asset_manager_interface.hpp"
#include "evie/core.h"
#include "evie/texture.h"

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

  Texture2DAsset GetTexture2D(const std::string& texture,
    TextureWrapping texture_wrapping = TextureWrapping::Repeat) override;

private:
  // Friend all AssetProxy types
  template<typename AssetType> friend class AssetProxy;

  // A helper struct to encapsulate the asset alongside a reference count.
  template<typename Asset> struct AssetHandle
  {
    AssetHandle() = default;
    AssetHandle(const AssetHandle& other) : asset(other.asset) { reference_count.store(other.reference_count); }

    AssetHandle(AssetHandle&& other) noexcept : asset(std::move(other.asset))
    {
      reference_count.store(other.reference_count);
    }

    AssetHandle& operator=(const AssetHandle& other)
    {
      if (this == &other) {
        return *this;
      }
      asset = other.asset;
      reference_count.store(other.reference_count);
      return *this;
    }

    AssetHandle& operator=(AssetHandle&& other) noexcept
    {
      asset = std::move(asset);
      reference_count.store(other.reference_count);
    }

    ~AssetHandle() = default;

    explicit AssetHandle(const Asset& asset_in) : asset(asset_in) {}

    // The actual asset data
    Asset asset;
    // The reference count to see the usage of this asset.
    std::atomic<int> reference_count{ 0 };
  };

  void IncreaseReference(AssetMetadata metadata) override;
  void DecreaseReference(AssetMetadata metadata) override;

  std::unordered_map<size_t, AssetHandle<Texture2D>> texture_2d_map_;
  std::filesystem::path asset_directory_;
};

}// namespace evie


#endif// !EVIE_ASSET_MANAGER_INCLUDE_ASSET_MANAGER_HPP_