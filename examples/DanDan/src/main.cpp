#include "evie/application.h"
#include "evie/entrypoint.h"
#include "evie/logging.h"


#include "game_layer.hpp"

class DanDanTheGame : public evie::Application
{
public:
  [[nodiscard]] evie::Error Init()
  {
    evie::WindowProperties props;
    props.name = "DanDanTheGame";
    props.dimensions.width = 1920;
    props.dimensions.height = 1080;
    APP_INFO("Initialising engine");
    evie::Error err = Initialise(props);
    GetWindow()->DisableCursor();
    if (err.Good()) {
      err = game_layer_.Initialise(GetInputManager(), GetECSController());
      if (err.Good()) {
        PushLayerBack(game_layer_);
      }
    }
    return err;
  }
  ~DanDanTheGame() override = default;

private:
  GameLayer game_layer_;
};

std::unique_ptr<evie::Application> CreateApplication()
{
  APP_INFO("Making Sandbox");
  std::unique_ptr<DanDanTheGame> app = std::make_unique<DanDanTheGame>();
  APP_INFO("Initialising Sandbox");
  evie::Error err = app->Init();
  if (err.Good()) {
    APP_INFO("Sandbox Initialised Succesfully");
    return app;
  } else {
    APP_INFO("App failed to initialise. Reason: {}", err.Message());
    return nullptr;
  }
}