#include "rendering/debug.h"
#include "evie/error.h"

namespace evie {
Error glCheckError()
{
  Error error = Error::OK();
  GLenum errorCode{ GL_NO_ERROR };
  while ((errorCode = glGetError()) != GL_NO_ERROR) {
    switch (errorCode) {
    case GL_INVALID_ENUM:
      error = Error{ "INVALID_ENUM" };
      break;
    case GL_INVALID_VALUE:
      error = Error{ "INVALID_VALUE" };
      break;
    case GL_INVALID_OPERATION:
      error = Error{ "INVALID_OPERATION" };
      break;
    case GL_OUT_OF_MEMORY:
      error = Error{ "OUT_OF_MEMORY" };
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      error = Error{ "INVALID_FRAMEBUFFER_OPERATION" };
      break;
    default:
      error = Error{ "UNKNOWN_OPENGL_ERROR" };
      break;
    }
  }
  return error;
}
}// namespace evie