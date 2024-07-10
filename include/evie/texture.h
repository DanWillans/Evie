#ifndef EVIE_RENDERING_TEXTURE_H_
#define EVIE_RENDERING_TEXTURE_H_

#include <string>

#include "evie/core.h"
#include "evie/error.h"
#include "evie/ids.h"

#include "glad/glad.h"

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