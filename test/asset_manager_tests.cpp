#include "doctest/doctest.h"

#include "asset_manager/asset_manager.hpp"
#include "evie/asset_manager_interface.hpp"
#include "evie/texture.h"


using namespace evie;

// NOLINTBEGIN

TEST_CASE("Test AssetManager")
{
  AssetManager asset_manager;
  Texture2DAsset texture_asset = asset_manager.GetTexture2D("dandan.png");
  REQUIRE(texture_asset.IsValid());
  REQUIRE(texture_asset.Get() != nullptr);
}
