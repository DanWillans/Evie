#ifndef EVIE_INCLUDE_VERTEX_BUFFER_H_
#define EVIE_INCLUDE_VERTEX_BUFFER_H_

#include <initializer_list>
#include <vector>

#include "evie/core.h"
#include "evie/error.h"
#include "evie/ids.h"
#include "evie/result.h"


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

class EVIE_API VertexBuffer
{
public:
  Error Initialise(const std::vector<float>& vertices_data, const BufferLayout& buffer_layout);
  void Bind();
  void Destroy();
  [[nodiscard]] const BufferLayout& GetBufferLayout() const { return buffer_layout_; }

private:
  std::vector<float> vertices_data_{};
  BufferLayout buffer_layout_{};
  VertexBufferID id_{ 0 };
};

}// namespace evie

#endif// !EVIE_INCLUDE_VERTEX_BUFFER_H_