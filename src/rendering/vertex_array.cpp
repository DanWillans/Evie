#include "evie/vertex_array.h"
#include "evie/error.h"
#include "evie/ids.h"
#include "evie/indices_array.h"
#include "evie/result.h"
#include "evie/vertex_buffer.h"
#include "rendering/debug.h"

#include "glad/glad.h"

namespace {
struct OpenGLTypeAndSize
{
  constexpr OpenGLTypeAndSize() = default;
  constexpr OpenGLTypeAndSize(GLenum type, size_t size) : type(type), size(size) {}
  GLenum type{ GL_FLOAT };
  size_t size{ 0 };
};

constexpr evie::Result<OpenGLTypeAndSize> ConvertVertexDataTypeToOpenGL(evie::VertexDataType type)
{
  switch (type) {
  case evie::VertexDataType::Byte:
    return { static_cast<GLenum>(GL_BYTE), (sizeof(std::byte)) };
  case evie::VertexDataType::UnsignedByte:
    return { static_cast<GLenum>(GL_UNSIGNED_BYTE), sizeof(unsigned char) };
  case evie::VertexDataType::Short:
    return { static_cast<GLenum>(GL_SHORT), sizeof(short) };
  case evie::VertexDataType::UnsignedShort:
    return { static_cast<GLenum>(GL_UNSIGNED_SHORT), sizeof(unsigned short) };
  case evie::VertexDataType::Int:
    return { static_cast<GLenum>(GL_INT), sizeof(int) };
  case evie::VertexDataType::Float:
    return { static_cast<GLenum>(GL_FLOAT), sizeof(float) };
  case evie::VertexDataType::Double:
    return { static_cast<GLenum>(GL_DOUBLE), sizeof(double) };
  default:
    return evie::Error{ "VertexDataType undefined" };
  }
}
}// namespace

namespace evie {

void VertexArray::Initialise()
{
  unsigned int VAO{ 0 };
  CallOpenGL(glGenVertexArrays, 1, &VAO);
  id_ = VertexArrayID(VAO);
  Bind();
}

Error VertexArray::AssociateVertexBuffer(VertexBuffer& vertex_buffer)
{
  // Bind this vertex array object
  Bind();
  // Bind the vertex buffer to associate it to the vertex array
  vertex_buffer.Bind();
  const auto& buffer_layout = vertex_buffer.GetBufferLayout();
  // Convert Evie layout type to OpenGL
  Result<OpenGLTypeAndSize> type_and_size;
  if (type_and_size = ConvertVertexDataTypeToOpenGL(buffer_layout.type); type_and_size.Bad()) {
    return type_and_size.Error();
  }
  // Iterate over the layout sizes of the buffer_layout and set attribute pointers
  for (size_t i = 0; i < buffer_layout.layout_sizes.size(); ++i) {
    CallOpenGL(glVertexAttribPointer,
      static_cast<GLuint>(i),
      buffer_layout.layout_sizes[i],
      type_and_size->type,
      false,
      static_cast<GLsizei>(buffer_layout.stride * type_and_size->size),
      reinterpret_cast<void*>(type_and_size->size * (i == 0 ? 0 : buffer_layout.layout_sizes[i - 1])));
    CallOpenGL(glEnableVertexAttribArray, static_cast<GLuint>(i));
  }
  return Error::OK();
}

void VertexArray::AssociateIndicesArray(IndicesArray& indices_array)
{
  Bind();
  indices_array.Bind();
}

void VertexArray::Bind() const { CallOpenGL(glBindVertexArray, id_.Get()); }

void VertexArray::Destroy() { CallOpenGL(glDeleteVertexArrays, 1, &id_.Get()); }

}// namespace evie