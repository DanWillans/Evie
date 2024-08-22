#ifndef EVIE_ASSET_MANAGER_INCLUDE_ASSET_MANAGER_INTERFACE_HPP_
#define EVIE_ASSET_MANAGER_INCLUDE_ASSET_MANAGER_INTERFACE_HPP_

#include <string>

namespace evie {

enum class AssetType : uint16_t {
  Texture2D,
};

struct AssetMetadata
{
  AssetType type;
  size_t hash;
};

template<typename AssetType> class AssetProxy;
class Texture2D;
class AssetMetadata;

class IAssetManager
{
public:
  virtual ~IAssetManager() = default;
  virtual AssetProxy<Texture2D> GetTexture2D(const std::string& texture_name) = 0;

private:
  template<typename AssetType> friend class AssetProxy;

  virtual void IncreaseReference(AssetMetadata handle) = 0;
  virtual void DecreaseReference(AssetMetadata handle) = 0;
};


template<typename AssetType> class AssetProxy
{
public:
  AssetProxy() = delete;
  ~AssetProxy() { asset_manager_->DecreaseReference(metadata_); }
  const AssetType* Get() { return asset_; }
  bool IsValid() { return valid_; }

private:
  friend class AssetManager;
  AssetProxy(IAssetManager* asset_manager, AssetMetadata metadata, AssetType* asset, bool valid = true)
    : asset_manager_(asset_manager), metadata_(metadata), asset_(asset), valid_(valid)
  {
    asset_manager_->IncreaseReference(metadata_);
  }
  AssetType* asset_;
  AssetMetadata metadata_;
  IAssetManager* asset_manager_;
  bool valid_{ false };
};

using Texture2DAsset = AssetProxy<Texture2D>;

}// namespace evie

#endif EVIE_ASSET_MANAGER_INCLUDE_ASSET_MANAGER_INTERFACE_HPP_