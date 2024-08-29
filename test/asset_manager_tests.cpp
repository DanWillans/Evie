#include "doctest/doctest.h"

#include "asset_manager/asset_manager.hpp"
#include "evie/asset_manager_interface.hpp"
#include "evie/shader_program.h"
#include "evie/texture.h"


// NOLINTBEGIN

namespace evie {
class AssetManagerTest : public IAssetManager
{
public:
  virtual ~AssetManagerTest() = default;
  AssetProxy<Texture2D> GetTexture2D(const std::string& texture_name,
    TextureWrapping texture_wrapping = TextureWrapping::Repeat) override
  {
    return AssetProxy<Texture2D>{ this, { AssetType::Texture2D, 0 }, &texture_2d };
  }

  AssetProxy<ShaderProgram> GetShaderProgram(const std::string& shader_name) override
  {
    return AssetProxy<ShaderProgram>{ this, { AssetType::ShaderProgram, 0 }, nullptr };
  }

  int GetTextureRefCount() { return texture_ref_count_; };

private:
  void IncreaseReference(AssetMetadata handle) override
  {
    if (handle.hash == 0) {
      texture_ref_count_++;
    } else {
      // Fail if handling any other handle so that we can support it
      REQUIRE(false);
    }
  };
  void DecreaseReference(AssetMetadata handle) override
  {
    if (handle.hash == 0) {
      texture_ref_count_--;
    } else {
      // Fail if handling any other handle so that we can support it
      REQUIRE(false);
    }
  };

  int texture_ref_count_{ 0 };
  Texture2D texture_2d;
};
}// namespace evie

TEST_CASE("Test AssetManager and AssetProxy interaction")
{
  evie::AssetManagerTest asset_manager;
  {
    evie::Texture2DAsset texture_asset = asset_manager.GetTexture2D("dandan.png");
    REQUIRE(texture_asset.IsValid());
    REQUIRE(texture_asset.Get() != nullptr);
    REQUIRE(asset_manager.GetTextureRefCount() == 1);

    // Test copy assignment reference counting
    {
      evie::Texture2DAsset texture_asset_copy_assignment;
      REQUIRE(!texture_asset_copy_assignment.IsValid());
      REQUIRE(texture_asset_copy_assignment.Get() == nullptr);
      REQUIRE(asset_manager.GetTextureRefCount() == 1);

      // Now copy assign the texture_asset;
      texture_asset_copy_assignment = texture_asset;
      REQUIRE(texture_asset_copy_assignment.IsValid());
      REQUIRE(texture_asset_copy_assignment.Get() != nullptr);
      REQUIRE(asset_manager.GetTextureRefCount() == 2);
    }

    // Check we're back now that the copy has been destructed
    REQUIRE(texture_asset.IsValid());
    REQUIRE(texture_asset.Get() != nullptr);
    REQUIRE(asset_manager.GetTextureRefCount() == 1);

    // Test copy constructor reference counting
    {
      evie::Texture2DAsset texture_asset_copy_constructor(texture_asset);
      REQUIRE(texture_asset_copy_constructor.IsValid());
      REQUIRE(texture_asset_copy_constructor.Get() != nullptr);
      REQUIRE(asset_manager.GetTextureRefCount() == 2);
    }

    // Check we're back now that the copy has been destructed
    REQUIRE(texture_asset.IsValid());
    REQUIRE(texture_asset.Get() != nullptr);
    REQUIRE(asset_manager.GetTextureRefCount() == 1);

    // Test move constructor reference counting
    evie::Texture2DAsset texture_asset_move_assignment;
    // Check that this isn't a valid asset first.
    REQUIRE(!texture_asset_move_assignment.IsValid());
    REQUIRE(texture_asset_move_assignment.Get() == nullptr);
    REQUIRE(asset_manager.GetTextureRefCount() == 1);

    // OK now move the texture_asset
    texture_asset_move_assignment = std::move(texture_asset);
    REQUIRE(texture_asset_move_assignment.IsValid());
    REQUIRE(texture_asset_move_assignment.Get() != nullptr);
    REQUIRE(asset_manager.GetTextureRefCount() == 1);

    // The moved texture_asset should now no longer be valid
    REQUIRE(!texture_asset.IsValid());
    REQUIRE(texture_asset.Get() == nullptr);
    REQUIRE(asset_manager.GetTextureRefCount() == 1);

    // Let's move construct from the move assignment
    evie::Texture2DAsset texture_asset_move_construct{ std::move(texture_asset_move_assignment) };
    REQUIRE(!texture_asset_move_assignment.IsValid());
    REQUIRE(texture_asset_move_assignment.Get() == nullptr);
    REQUIRE(texture_asset_move_construct.IsValid());
    REQUIRE(texture_asset_move_construct.Get() != nullptr);
    REQUIRE(asset_manager.GetTextureRefCount() == 1);
  }
  REQUIRE(asset_manager.GetTextureRefCount() == 0);

  // Let's create a bunch of copies and ensure reference counting is working
  std::vector<evie::Texture2DAsset> texture_copies{};
  evie::Texture2DAsset texture_asset = asset_manager.GetTexture2D("dandan.png");
  size_t copy_amount = 100;
  for (size_t i = 0; i < copy_amount; ++i) {
    texture_copies.push_back(texture_asset);
  }

  // Check all the texture assets are valid.
  for (const auto& texture : texture_copies) {
    REQUIRE(texture.IsValid());
    REQUIRE(texture.Get() != nullptr);
  }

  // Check that we have 101 references of this asset.
  REQUIRE(asset_manager.GetTextureRefCount() == 101);
}
