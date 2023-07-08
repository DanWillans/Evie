#ifndef EVIE_EVENT_MANAGER_H_
#define EVIE_EVENT_MANAGER_H_

#include <memory>

#include "events.h"

namespace evie {

class EVIE_API IEventListener
{
public:
  IEventListener() = default;
  IEventListener(const IEventListener&) = default;
  IEventListener(IEventListener&&) = default;
  IEventListener& operator=(IEventListener&&) = default;
  IEventListener& operator=(const IEventListener&) = default;
  virtual ~IEventListener() = default;

  virtual void OnEvent(std::unique_ptr<Event> event) = 0;
};

/**
 * @brief A class that manages all input events into the engine.
 *
 */
class EVIE_API EventManager final : public IEventListener
{
public:
  void OnEvent(std::unique_ptr<Event> event) override;
};
}// namespace evie

#endif// !EVIE_EVENT_MANAGER_H_