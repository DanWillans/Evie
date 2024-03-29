#ifndef EVIE_IDS_H_
#define EVIE_IDS_H_

namespace evie {
// A generic class that can be used as a unique identifier
// Tag's are used so that different Aliased IDs cannot be compared
template<typename T, typename Tag> class ID
{
public:
  constexpr explicit ID(const T& identifier) : id_(identifier) {}

  constexpr bool operator==(const ID<T, Tag>& other) const { return id_ == other.Get(); }

  T Get() const { return id_; }

private:
  T id_;
};

// Aliases of ID
struct LayerIDTag
{
};
using LayerID = ID<int, LayerIDTag>;

}// namespace evie

#endif// EVIE_IDS_H_