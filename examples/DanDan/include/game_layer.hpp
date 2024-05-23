#ifndef INCLUDE_GAME_LAYER_HPP_
#define INCLUDE_GAME_LAYER_HPP_

#include <evie/layer.h>
#include <evie/error.h>

class GameLayer final : public evie::Layer 
{
  public:
  evie::Error Initialise();
  void OnUpdate() override;
  void OnRender() override;
  void OnEvent(evie::Event& event) override;
};

#endif// !INCLUDE_GAME_LAYER_HPP_