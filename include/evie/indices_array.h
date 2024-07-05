#ifndef EVIE_INCLUDE_INDICES_ARRAY_H_
#define EVIE_INCLUDE_INDICES_ARRAY_H_

#include <vector>

#include "evie/core.h"
#include "evie/ids.h"

namespace evie {

class EVIE_API IndicesArray
{
public:
  void Initialise(const std::vector<unsigned int>& indices);
  void Bind();
  void Destroy();

private:
  std::vector<unsigned int> indices_{};
  IndicesArrayID id_{ 0 };
};

}// namespace evie


#endif// !EVIE_INCLUDE_INDICES_ARRAY_H_