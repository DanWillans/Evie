#ifndef EVIE_IDS_H_
#define EVIE_IDS_H_

namespace evie {
// A generic class that can be used to as a unique identifier
template<typename T> class ID
{
public:
  explicit ID(const T& identifier) : id_(identifier) {}

  bool operator==(const ID& other) const { return id_ == other.Get(); }

  T Get() const { return id_; }

private:
  T id_;
};

// Aliases of ID
using LayerID = ID<int>;

}// namespace evie

#endif// EVIE_IDS_H_