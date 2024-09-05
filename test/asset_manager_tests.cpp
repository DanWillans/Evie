#include "doctest/doctest.h"

#include "asset_manager/asset_manager.hpp"
#include "evie/asset_manager_interface.hpp"
#include "evie/shader_program.h"
#include "evie/texture.h"


// NOLINTBEGIN

namespace evie {
class AssetManagerTest
  : public IAssetManager
{
public:
  virtual ~AssetManagerTest() = default;
  Result<Texture2DAsset> GetTexture2D(const std::string& texture_name,
    TextureWrapping texture_wrapping = TextureWrapping::Repeat) override
  {
    ref_count_vec_.push_back(0);
    return AssetProxy<Texture2D>{ this, { AssetType::Texture2D, count_++ }, &texture_2d };
  }

  Result<ShaderProgramAsset> GetShaderProgram(const std::string& shader_name) override
  {
    return AssetProxy<ShaderProgram>{ this, { AssetType::ShaderProgram, 0 }, nullptr };
  }

  Result<ModelAsset> GetModel(const std::string& model_name) override
  {
    return AssetProxy<Model>{ shared_from_this(), { AssetType::Model, 0 }, nullptr };
  }

  Result<ModelAsset> GetModel(default_models::PrimitiveModel model_type,
    const std::vector<Texture2DAsset>& textures) override
  {
    return AssetProxy<Model>{ shared_from_this(), { AssetType::PrimitiveModel, 0 }, nullptr };
  }

  template<typename T> int GetTextureRefCount(const AssetProxy<T>& handle)
  {
    return ref_count_vec_[handle.metadata_.hash];
  }

private:
  void IncreaseReference(AssetMetadata handle) override { ref_count_vec_[handle.hash]++; };
  void DecreaseReference(AssetMetadata handle) override { ref_count_vec_[handle.hash]--; };

  size_t count_{ 0 };
  std::vector<int> ref_count_vec_{};
  Texture2D texture_2d;
};
}// namespace evie

TEST_CASE("Test AssetManager and AssetProxy interaction")
{
  std::shared_ptr<evie::AssetManagerTest> asset_manager = std::make_unique<evie::AssetManagerTest>();
  {
    evie::Result<evie::Texture2DAsset> texture_asset = asset_manager->GetTexture2D("dandan.jpg");
    REQUIRE(texture_asset.Good());
    REQUIRE(texture_asset->IsValid());
    REQUIRE(texture_asset->Get() != nullptr);
    REQUIRE(asset_manager->GetTextureRefCount(*texture_asset) == 1);

    // Test copy assignment reference counting
    {
      evie::Texture2DAsset texture_asset_copy_assignment;
      REQUIRE(!texture_asset_copy_assignment.IsValid());
      REQUIRE(texture_asset_copy_assignment.Get() == nullptr);
      REQUIRE(asset_manager->GetTextureRefCount(*texture_asset) == 1);

      // Now copy assign the texture_asset;
      texture_asset_copy_assignment = *texture_asset;
      REQUIRE(texture_asset_copy_assignment.IsValid());
      REQUIRE(texture_asset_copy_assignment.Get() != nullptr);
      REQUIRE(asset_manager->GetTextureRefCount(texture_asset_copy_assignment) == 2);
    }

    // Check we're back now that the copy has been destructed
    REQUIRE(texture_asset->IsValid());
    REQUIRE(texture_asset->Get() != nullptr);
    REQUIRE(asset_manager->GetTextureRefCount(*texture_asset) == 1);

    // Test copy constructor reference counting
    {
      evie::Texture2DAsset texture_asset_copy_constructor(*texture_asset);
      REQUIRE(texture_asset_copy_constructor.IsValid());
      REQUIRE(texture_asset_copy_constructor.Get() != nullptr);
      REQUIRE(asset_manager->GetTextureRefCount(texture_asset_copy_constructor) == 2);
    }

    // Check we're back now that the copy has been destructed
    REQUIRE(texture_asset->IsValid());
    REQUIRE(texture_asset->Get() != nullptr);
    REQUIRE(asset_manager->GetTextureRefCount(*texture_asset) == 1);

    // Test move constructor reference counting
    evie::Texture2DAsset texture_asset_move_assignment;
    // Check that this isn't a valid asset first.
    REQUIRE(!texture_asset_move_assignment.IsValid());
    REQUIRE(texture_asset_move_assignment.Get() == nullptr);
    REQUIRE(asset_manager->GetTextureRefCount(*texture_asset) == 1);

    // OK now move the texture_asset
    texture_asset_move_assignment = std::move(*texture_asset);
    REQUIRE(texture_asset_move_assignment.IsValid());
    REQUIRE(texture_asset_move_assignment.Get() != nullptr);
    REQUIRE(asset_manager->GetTextureRefCount(texture_asset_move_assignment) == 1);

    // The moved texture_asset should now no longer be valid
    REQUIRE(!texture_asset->IsValid());
    REQUIRE(texture_asset->Get() == nullptr);
    REQUIRE(asset_manager->GetTextureRefCount(*texture_asset) == 1);

    // Let's move construct from the move assignment
    evie::Texture2DAsset texture_asset_move_construct{ std::move(texture_asset_move_assignment) };
    REQUIRE(!texture_asset_move_assignment.IsValid());
    REQUIRE(texture_asset_move_assignment.Get() == nullptr);
    REQUIRE(texture_asset_move_construct.IsValid());
    REQUIRE(texture_asset_move_construct.Get() != nullptr);
    REQUIRE(asset_manager->GetTextureRefCount(texture_asset_move_construct) == 1);
  }

  // Let's create a bunch of copies and ensure reference counting is working
  std::vector<evie::Texture2DAsset> texture_copies{};
  evie::Result<evie::Texture2DAsset> texture_asset = asset_manager->GetTexture2D("dandan.png");
  REQUIRE(asset_manager->GetTextureRefCount(*texture_asset) == 1);
  REQUIRE(texture_asset.Good());
  size_t copy_amount = 100;
  for (size_t i = 0; i < copy_amount; ++i) {
    texture_copies.push_back(*texture_asset);
  }

  // Check all the texture assets are valid.
  for (const auto& texture : texture_copies) {
    REQUIRE(texture.IsValid());
    REQUIRE(texture.Get() != nullptr);
  }

  // Check that we have 101 references of this asset.
  REQUIRE(asset_manager->GetTextureRefCount(*texture_asset) == 101);
}

TEST_CASE("Test AssetProxy overriden in vector")
{
  std::shared_ptr<evie::AssetManagerTest> asset_manager = std::make_unique<evie::AssetManagerTest>();
  evie::Result<evie::Texture2DAsset> texture_asset = asset_manager->GetTexture2D("dandan.png");

  REQUIRE(texture_asset);

  std::vector<evie::Texture2DAsset> result;
  result.push_back(*texture_asset);
  result.push_back(*texture_asset);
  result.push_back(*texture_asset);
  result.push_back(*texture_asset);
  REQUIRE(asset_manager->GetTextureRefCount(*texture_asset) == 5);

  evie::Result<evie::Texture2DAsset> texture_asset_2 = asset_manager->GetTexture2D("dandan.png");
  REQUIRE(asset_manager->GetTextureRefCount(*texture_asset_2) == 1);

  result[1] = *texture_asset_2;
  // The original asset was overwritten with a different asset so it should decrease ref count
  REQUIRE(asset_manager->GetTextureRefCount(*texture_asset) == 4);
  // The new asset was copied so should increase ref count
  REQUIRE(asset_manager->GetTextureRefCount(*texture_asset_2) == 2);

  result[1] = *texture_asset;
  REQUIRE(asset_manager->GetTextureRefCount(*texture_asset) == 5);
  REQUIRE(asset_manager->GetTextureRefCount(*texture_asset_2) == 1);

  result[1] = *texture_asset;
  // Nothing should happen to the reference counts
  REQUIRE(asset_manager->GetTextureRefCount(*texture_asset) == 5);
  REQUIRE(asset_manager->GetTextureRefCount(*texture_asset_2) == 1);
}
