#ifndef EVIE_ASSET_MANAGER_INCLUDE_ASSET_TYPES_HPP_
#define EVIE_ASSET_MANAGER_INCLUDE_ASSET_TYPES_HPP_

#include <cstdint>

namespace evie {

enum class AssetType : uint16_t {
  Texture2D,
};

struct AssetMetadata
{
  AssetType type;
  size_t hash;
};

}// namespace evie

#endif EVIE_ASSET_MANAGER_INCLUDE_ASSET_TYPES_HPP_