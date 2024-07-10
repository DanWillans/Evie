#include "evie/indices_array.h"
#include "evie/ids.h"
#include "rendering/debug.h"


#include "glad/glad.h"

namespace evie {
void IndicesArray::Initialise(const std::vector<unsigned int>& indices)
{
  unsigned int EBO{ 0 };
  CallOpenGL(glGenBuffers, 1, &EBO);
  id_ = IndicesArrayID(EBO);
  indices_ = indices;
  Bind();
  CallOpenGL(glBufferData, GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices_.data(), GL_STATIC_DRAW);
}

void IndicesArray::Bind() { CallOpenGL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, id_.Get()); }

void IndicesArray::Destroy() { CallOpenGL(glDeleteBuffers, 1, &id_.Get()); }

}// namespace evie