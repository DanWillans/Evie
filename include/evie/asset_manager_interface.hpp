#ifndef EVIE_ASSET_MANAGER_INCLUDE_ASSET_MANAGER_INTERFACE_HPP_
#define EVIE_ASSET_MANAGER_INCLUDE_ASSET_MANAGER_INTERFACE_HPP_

#include <string>

#include "logging.h"

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
  AssetProxy() { EV_INFO("Default constructor"); };
  AssetProxy(const AssetProxy& other)
    : asset_(other.asset_), metadata_(other.metadata_), asset_manager_(other.asset_manager_), valid_(other.valid_)
  {
    EV_INFO("Copy constructor");
    // Only incrase reference if this is a valid asset.
    if (valid_) {
      asset_manager_->IncreaseReference(metadata_);
    }
  }
  AssetProxy& operator=(AssetProxy&& other)
  {
    EV_INFO("Move assignment");
    asset_ = other.asset_;
    metadata_ = other.metadata_;
    asset_manager_ = other.asset_manager_;
    valid_ = other.valid_;
    other.asset_manager_ = nullptr;
    other.asset_ = nullptr;
    // Don't touch metadata
    other.valid_ = false;
    // Don't decrease or increase reference. We're moving the proxy so "other" is on longer useable anymore.
    return *this;
  }
  AssetProxy& operator=(const AssetProxy& other)
  {
    EV_INFO("Copy assignment");
    asset_ = other.asset_;
    metadata_ = other.metadata_;
    asset_manager_ = other.asset_manager_;
    valid_ = other.valid_;
    return *this;
  }
  AssetProxy(AssetProxy&& other) noexcept
    : asset_(other.asset_), metadata_(other.metadata_), asset_manager_(other.asset_manager_), valid_(other.valid_)
  {
    EV_INFO("Move constructor");
    other.asset_manager_ = nullptr;
    other.asset_ = nullptr;
    // Don't touch metadata
    other.valid_ = false;
    // Don't decrease or increase reference. We're moving the proxy so "other" is on longer useable anymore.
  }
  ~AssetProxy()
  {
    EV_INFO("Destructor");
    // Only decrease reference if this is a valid asset
    if (valid_) {
      asset_manager_->DecreaseReference(metadata_);
    }
  }
  const AssetType* Get() { return asset_; }
  bool IsValid() { return valid_; }

private:
  friend class AssetManager;
  AssetProxy(IAssetManager* asset_manager, AssetMetadata metadata, AssetType* asset, bool valid = true)
    : asset_manager_(asset_manager), metadata_(metadata), asset_(asset), valid_(valid)
  {
    EV_INFO("Private construtor");
    asset_manager_->IncreaseReference(metadata_);
  }
  AssetType* asset_{ nullptr };
  AssetMetadata metadata_;
  IAssetManager* asset_manager_{ nullptr };
  bool valid_{ false };
};

using Texture2DAsset = AssetProxy<Texture2D>;

}// namespace evie

#endif EVIE_ASSET_MANAGER_INCLUDE_ASSET_MANAGER_INTERFACE_HPP_