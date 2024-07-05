#ifndef EVIE_INCLUDE_VERTEX_BUFFER_H_
#define EVIE_INCLUDE_VERTEX_BUFFER_H_

#include <numeric>
#include <vector>

#include "evie/core.h"
#include "evie/error.h"
#include "evie/ids.h"
#include "rendering/debug.h"

#include "glad/glad.h"

namespace evie {

enum class VertexDataType { Byte, UnsignedByte, Short, UnsignedShort, Int, Float, Double };

struct BufferLayout
{
  BufferLayout() = default;
  BufferLayout(uint16_t stride, VertexDataType type, const std::vector<uint16_t>& layout_sizes)
    : stride(stride), type(type), layout_sizes(layout_sizes)
  {}
  uint16_t stride{ 0 };
  VertexDataType type{ VertexDataType::Float };
  std::vector<uint16_t> layout_sizes;
};

template<typename T = float> class EVIE_API VertexBuffer
{
public:
  Error Initialise(const std::vector<T>& vertices_data, const BufferLayout& buffer_layout);
  void Bind();
  void Destroy();
  [[nodiscard]] const BufferLayout& GetBufferLayout() const { return buffer_layout_; }

  void UpdateBuffer(const std::vector<T>& vertices_data);

  [[nodiscard]] const std::vector<T>& GetBuffer() const { return vertices_data_; }

private:
  std::vector<T> vertices_data_{};
  BufferLayout buffer_layout_{};
  VertexBufferID id_{ 0 };
};

template<typename T>
Error VertexBuffer<T>::Initialise(const std::vector<T>& vertices_data, const BufferLayout& buffer_layout)
{
  unsigned int VBO{ 0 };
  CallOpenGL(glGenBuffers, 1, &VBO);
  id_ = VertexBufferID(VBO);
  Bind();
  if (buffer_layout.layout_sizes.empty()) {
    return Error{ "Buffer layout_sizes is empty" };
  }
  // Check that the sum of the buffer layout equals the stride
  int sum = std::accumulate(buffer_layout.layout_sizes.begin(), buffer_layout.layout_sizes.end(), 0);
  if (sum != buffer_layout.stride) {
    return Error{ "The sum of the elements in the buffer layout don't match the stride" };
  }
  // Check that the vertices data size is a multiple of buffer layout size
  if (vertices_data.size() % buffer_layout.stride != 0) {
    return Error{ "Vertices data size isn't a multiple of the buffer layout" };
  }
  buffer_layout_ = buffer_layout;
  vertices_data_ = vertices_data;
  CallOpenGL(glBufferData, GL_ARRAY_BUFFER, vertices_data.size() * sizeof(T), vertices_data.data(), GL_STATIC_DRAW);
  return Error::OK();
}

template<typename T> void VertexBuffer<T>::Bind() { CallOpenGL(glBindBuffer, GL_ARRAY_BUFFER, id_.Get()); }

template<typename T> void VertexBuffer<T>::Destroy() { CallOpenGL(glDeleteBuffers, 1, &id_.Get()); }

template<typename T> void VertexBuffer<T>::UpdateBuffer(const std::vector<T>& vertices_data)
{
  Bind();
  CallOpenGL(glBufferSubData, GL_ARRAY_BUFFER, 0, vertices_data.size() * sizeof(T), vertices_data.data());
}

}// namespace evie

#endif// !EVIE_INCLUDE_VERTEX_BUFFER_H_