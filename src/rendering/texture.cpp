#include <iostream>
#include <string>

#include "evie/texture.h"
#include "evie/ids.h"
#include "evie/result.h"
#include "rendering/debug.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace {

constexpr evie::Result<GLenum> ConvertToOpenGLFormat(evie::ImageFormat format)
{
  switch (format) {
  case evie::ImageFormat::RGB:
    return static_cast<unsigned int>(GL_RGB);
  case evie::ImageFormat::RGBA:
    return static_cast<unsigned int>(GL_RGBA);
  default:
    return "Unsupported conversion";
  };
}

};// namespace

namespace evie {

Error Texture2D::Initialise(const std::string& filename, ImageFormat format, bool flip)
{
  Error err = Error::OK();
  if (flip) {
    stbi_set_flip_vertically_on_load(1);
  }
  unsigned char* data = nullptr;

  Result<GLenum> ogl_format = ConvertToOpenGLFormat(format);
  if (ogl_format.Bad()) {
    return ogl_format.Error();
  }

  data = stbi_load(filename.c_str(), &width_, &height_, &number_of_channels_, 0);
  if (data) {
    // Generate the texture
    unsigned int id;
    CallOpenGL(glGenTextures, 1, &id);
    // Update the ID
    id_ = TextureID(id);
    // Bind the texture in openGL state machine
    this->Bind();
    // Set wrapping parameters. This is the default for now. Will extend the class
    // in the future to allow customisation of this.
    CallOpenGL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    CallOpenGL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    CallOpenGL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    CallOpenGL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Generate the 2D Texture Image in openGL
    CallOpenGL(glTexImage2D, GL_TEXTURE_2D, 0, *ogl_format, width_, height_, 0, *ogl_format, GL_UNSIGNED_BYTE, data);
    // Get openGL to generate the MipMap of the texture for us.
    CallOpenGL(glGenerateMipmap, GL_TEXTURE_2D);
    stbi_image_free(data);
  } else {
    err = Error{ "Failed to load texture" };
  }
  return err;
}

void Texture2D::SetSlot(int slot)
{
  CallOpenGL(glActiveTexture, GL_TEXTURE0 + slot);
  this->Bind();
}

void Texture2D::Bind() { CallOpenGL(glBindTexture, GL_TEXTURE_2D, id_.Get()); }

void Texture2D::Destroy() { CallOpenGL(glDeleteTextures, 1, &id_.Get()); }
}// namespace evie
