#ifndef EVIE_IDS_H_
#define EVIE_IDS_H_

#include <cstdint>
#include <functional>

namespace evie {
// A generic class that can be used as a unique identifier
// Tag's are used so that different Aliased IDs cannot be compared
template<typename T, typename Tag> class ID
{
public:
  constexpr explicit ID(const T& identifier) : id_(identifier) {}

  constexpr bool operator==(const ID<T, Tag>& other) const { return id_ == other.Get(); }

  const T& Get() const { return id_; }

private:
  T id_;
};

// Aliases of ID
struct LayerIDTag
{
};
using LayerID = ID<int, LayerIDTag>;

struct ShaderIdTag
{
};
using ShaderID = ID<unsigned int, ShaderIdTag>;

struct ShaderProgramTag
{
};
using ShaderProgramID = ID<unsigned int, ShaderProgramTag>;

struct TextureTag
{
};
using TextureID = ID<unsigned int, TextureTag>;

struct VertexArrayTag
{
};
using VertexArrayID = ID<unsigned int, VertexArrayTag>;

struct VertexBufferTag
{
};
using VertexBufferID = ID<unsigned int, VertexBufferTag>;

struct IndicesArrayTag
{
};
using IndicesArrayID = ID<unsigned int, IndicesArrayTag>;

struct EntityTag
{
};
using EntityID = ID<uint64_t, EntityTag>;

template<typename ComponentName> using ComponentID = ID<uint64_t, ComponentName>;

template<typename SystemName> using SystemID = ID<uint64_t, SystemName>;

}// namespace evie

namespace std {
template<typename T, typename Tag> struct hash<evie::ID<T, Tag>>
{
  size_t operator()(const evie::ID<T, Tag>& id) const noexcept { return id.Get(); }
};
}// namespace std

#endif// EVIE_IDS_H_