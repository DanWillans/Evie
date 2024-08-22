#ifndef EVIE_ASSET_MANAGER_INCLUDE_ASSET_MANAGER_INTERFACE_HPP_
#define EVIE_ASSET_MANAGER_INCLUDE_ASSET_MANAGER_INTERFACE_HPP_

#include "asset_types.hpp"

namespace evie {

class IAssetManager
{
private:
  template<typename AssetType> friend class AssetProxy;

  virtual void IncreaseReference(AssetMetadata handle) = 0;
  virtual void DecreaseReference(AssetMetadata handle) = 0;
};

}// namespace evie

#endif EVIE_ASSET_MANAGER_INCLUDE_ASSET_MANAGER_INTERFACE_HPP_