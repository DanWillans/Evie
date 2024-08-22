#ifndef EVIE_RENDERING_TEXTURE_H_
#define EVIE_RENDERING_TEXTURE_H_

#include <string>

#include "evie/core.h"
#include "evie/error.h"
#include "evie/ids.h"

namespace evie {

enum class TextureType { Diffuse, Specular };

enum class TextureWrapping { Repeat, MirroredRepeat, ClampToEdge, ClampToBorder };
class EVIE_API Texture2D
{
public:
  Error Initialise(const std::string& filename, bool flip = false, TextureWrapping wrapping = TextureWrapping::Repeat);
  void SetSlot(int slot);
  void Bind();
  void Destroy();

  Texture2D& operator=(const Texture2D& other)
  {
    id_ = other.id_;
    texture_slot_ = other.texture_slot_;
    width_ = other.width_;
    height_ = other.height_;
    number_of_channels_ = other.number_of_channels_;
    printf("other.name_ size: %d\n", other.name_.size());
    name_ = other.name_;
    return *this;
  }

  TextureType type{ TextureType::Diffuse };

  const std::string& GetName() const { return name_; }

  TextureID GetID() const { return id_; }

private:
  TextureID id_{ 0 };
  int texture_slot_{ 0 };
  int width_{ 0 };
  int height_{ 0 };
  int number_of_channels_{ 0 };
  std::string name_{};
};
}// namespace evie

#endif// !EVIE_RENDERING_TEXTURE_H_