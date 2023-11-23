#include <iostream>
#include <numeric>
#include <string>


#include "evie/core.h"
#include "evie/error.h"
#include "evie/ids.h"
#include "evie/vertex_buffer.h"
#include "rendering/debug.h"


#include "glad/glad.h"

namespace evie {


Error VertexBuffer::Initialise(const std::vector<float>& vertices_data, const BufferLayout& buffer_layout)
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
  CallOpenGL(glBufferData, GL_ARRAY_BUFFER, vertices_data.size() * sizeof(float), vertices_data.data(), GL_STATIC_DRAW);
  return Error::OK();
}

void VertexBuffer::Bind() { CallOpenGL(glBindBuffer, GL_ARRAY_BUFFER, id_.Get()); }

void VertexBuffer::Destroy() { CallOpenGL(glDeleteBuffers, 1, &id_.Get()); }

}// namespace evie