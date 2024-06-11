#ifndef INCLUDE_COMPONENTS_HPP_
#define INCLUDE_COMPONENTS_HPP_

// Empty component, purely used to tag an entity as a follow target
#include <evie/types.h>
class FollowTargetComponent
{
};

// Empty component, purely used to tag an entity as a entity that follows a target.
class FollowerComponent
{
};

class ProjectileComponent
{
};

class VelocityComponent
{
  public:
  evie::vec3 velocity;
};

#endif// !INCLUDE_COMPONENTS_HPP_