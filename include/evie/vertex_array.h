#ifndef EVIE_INCLUDE_VERTEX_ARRAY_H_
#define EVIE_INCLUDE_VERTEX_ARRAY_H_

#include <initializer_list>
#include <vector>

#include "evie/error.h"
#include "evie/ids.h"
#include "evie/indices_array.h"
#include "evie/vertex_buffer.h"



namespace evie {
class EVIE_API VertexArray
{
public:
  void Initialise();
  Error AssociateVertexBuffer(VertexBuffer& vertex_buffer);
  void AssociateIndicesArray(IndicesArray& indices_array);
  void Bind();
  void Destroy();

private:
  VertexBuffer* vertex_buffer_{ nullptr };
  IndicesArray* indices_array_{ nullptr };
  VertexArrayID id_{ 0 };
};
}// namespace evie

#endif// !evie_include_vertex_array_h_