#ifndef EVIE_ASSET_MANAGER_INCLUDE_ASSET_MANAGER_INTERFACE_HPP_
#define EVIE_ASSET_MANAGER_INCLUDE_ASSET_MANAGER_INTERFACE_HPP_

#include <evie/model.hpp>
#include <evie/shader_program.h>
#include <evie/texture.h>
#include <string>

namespace evie {

/**
 * @brief All supported asset types in Evie.
 *
 */
enum class AssetType : uint16_t {
  Texture2D,
  ShaderProgram,
  Model
};

/**
 * @brief A struct that stores information/metadata about a particular asset. Used by the AssetManager to link
 * AssetProxy instances back to particular assets.
 *
 */
struct AssetMetadata
{
  AssetType type;
  // A unique identifier for a particular asset
  size_t hash;
  auto operator<=>(const AssetMetadata&) const = default;
};

// Forward declaration
template<typename AssetType> class AssetProxy;
struct AssetMetadata;

// AssetProxy aliases
using Texture2DAsset = AssetProxy<Texture2D>;
using ShaderProgramAsset = AssetProxy<ShaderProgram>;
using ModelAsset = AssetProxy<Model>;

/**
 * @brief Pure abstract class for AssetManager. Any AssetManager implementation must derive from this interface.
 *
 */
class IAssetManager
{
public:
  virtual ~IAssetManager() = default;
  /**
   * @brief Get and/or load a Texture2D object. This will search for a texture with the provided texture_name argument.
   *
   * @param texture_name The name of the texture to load
   * @param texture_wrapping The wrapping required for the texture
   * @return Result<Texture2DAsset> A Texture2DAsset Result.
   */
  virtual Result<Texture2DAsset> GetTexture2D(const std::string& texture_name,
    TextureWrapping texture_wrapping = TextureWrapping::Repeat) = 0;

  // This is just temporary until Renderer code is written. No need for users to get shader programs at the moment.
  virtual Result<ShaderProgramAsset> GetShaderProgram(const std::string& shader_name) = 0;

  /**
   * @brief Get and/or load a Model object. This will search for a model with the provided model name argument.
   *
   * @param model_name The name of the model to load.
   * @return Result<ModelAsset> A ModelAsset Result.
   */
  virtual Result<ModelAsset> GetModel(const std::string& model_name) = 0;

private:
  template<typename AssetType> friend class AssetProxy;

  virtual void IncreaseReference(AssetMetadata handle) = 0;
  virtual void DecreaseReference(AssetMetadata handle) = 0;
};


/**
 * @brief This class provides proxy access to assets loaded via an AssetManager. The intention behind this class is to
 * allow multiple non owning references to an asset. This AssetProxy will be returned by an AssetManager to a loaded
 * asset.
 *
 * The design decision behind an AssetProxy is so that the AssetManager owns all the assets and can control what to do
 * with those assets. The AssetManager can decide to lazy-load assets, deallocate/destroy the asset if there are no more
 * references etc.
 *
 * The AssetProxy can be safely copied or moved around and will interact with the AssetManager appropriately to
 * increse/decrease reference counts.
 *
 * @tparam AssetType The type of the asset that this class exposes.
 */
template<typename AssetType> class AssetProxy
{
public:
  // Default constructor
  AssetProxy() = default;

  // Copy constructor
  AssetProxy(const AssetProxy& other)
    : asset_(other.asset_), metadata_(other.metadata_), asset_manager_(other.asset_manager_), valid_(other.valid_)
  {
    // Only increase reference if this is a valid asset.
    if (valid_) {
      asset_manager_->IncreaseReference(metadata_);
    }
  }

  // Move assignment
  AssetProxy& operator=(AssetProxy&& other) noexcept
  {
    asset_ = other.asset_;
    metadata_ = other.metadata_;
    asset_manager_ = other.asset_manager_;
    valid_ = other.valid_;
    other.asset_ = nullptr;
    // Don't touch metadata
    other.valid_ = false;
    // Don't decrease or increase reference. We're moving the proxy so "other" is on longer useable anymore.
    return *this;
  }

  // Copy assignment
  AssetProxy& operator=(const AssetProxy& other)
  {
    if (this == &other) {
      return *this;
    }
    // If the new AssetProxy metadata differs then we should
    // decrease the one we previously proxied because we're losing a reference.
    if (valid_) {
      if (metadata_ != other.metadata_) {
        asset_manager_->DecreaseReference(metadata_);
      } else {
        // Do nothing. The original assetproxy and the one to copy point to the same asset.
        return *this;
      }
    }

    asset_ = other.asset_;
    metadata_ = other.metadata_;
    asset_manager_ = other.asset_manager_;
    valid_ = other.valid_;

    // Only increase reference if this is a valid asset.
    if (valid_) {
      asset_manager_->IncreaseReference(metadata_);
    }
    return *this;
  }

  // Move constructor
  AssetProxy(AssetProxy&& other) noexcept
    : asset_(other.asset_), metadata_(other.metadata_), asset_manager_(other.asset_manager_), valid_(other.valid_)
  {
    other.asset_ = nullptr;
    // Don't touch metadata
    // Don't decrease or increase reference. We're moving the AssetProxy so "other" is no longer useable anymore.
    other.valid_ = false;
  }

  // Destructor
  ~AssetProxy()
  {
    // Only decrease reference if this is a valid asset
    if (valid_) {
      asset_manager_->DecreaseReference(metadata_);
    }
  }
  AssetType* Get() { return asset_; }
  const AssetType* Get() const { return asset_; }
  bool IsValid() const { return valid_; }

  bool operator()() { return valid_; }

  /**
   * @brief Manually destroy and cleanup the proxy instance. Generally the destructor will handle all of this for you
   * but there may be instances where you want to destroy the proxy yourself.
   *
   */
  void Destroy()
  {
    if (valid_) {
      asset_manager_->DecreaseReference(metadata_);
    }
    asset_ = nullptr;
    valid_ = false;
    // Don't touch metadata because there's no reasonable data to set it to. valid_ is also false so nothing should be
    // handling it.
  }

private:
  friend class AssetManager;
  friend class AssetManagerTest;// Only for test. Not the nicest solution but I wanted to move on.

  /**
   * @brief Construct a new AssetProxy object. This constructor is only to be called by friend classes and is the only
   * way to create a valid AssetProxy object.
   *
   * @param asset_manager An instance of an IAssetManager that owns the asset
   * @param metadata Metadata about this AssetProxy
   * @param asset A pointer to the asset that this Proxy will expose.
   * @param valid Determines where this proxy is valid or not
   */
  AssetProxy(IAssetManager* asset_manager, AssetMetadata metadata, AssetType* asset, bool valid = true)
    : asset_manager_(asset_manager), metadata_(metadata), asset_(asset), valid_(valid)
  {
    if (valid_) {
      asset_manager_->IncreaseReference(metadata_);
    }
  }

  AssetType* asset_{ nullptr };
  AssetMetadata metadata_{};
  IAssetManager* asset_manager_;
  bool valid_{ false };
};


}// namespace evie

#endif// EVIE_ASSET_MANAGER_INCLUDE_ASSET_MANAGER_INTERFACE_HPP_