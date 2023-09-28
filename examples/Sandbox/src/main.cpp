#include <memory>

#include "evie/application.h"
#include "evie/error.h"
#include "evie/events.h"
#include "evie/evie.h"
#include "evie/input.h"
#include "evie/input_manager.h"
#include "evie/layer.h"
#include "evie/logging.h"
#include "evie/types.h"

class TestLayer : public evie::Layer
{
public:
  TestLayer() = default;
  explicit TestLayer(const evie::IInputManager* input_manager) : input_manager_(input_manager) {}
  TestLayer(const TestLayer&) = default;
  TestLayer& operator=(const TestLayer&) = default;
  TestLayer(TestLayer&&) = delete;
  ~TestLayer() override = default;
  void OnUpdate() override
  {
    if (input_manager_ && input_manager_->IsKeyPressed(evie::EvieKey::Space)) {
      APP_INFO("Space bar pressed");
    }
  }
  void OnEvent([[maybe_unused]] evie::Event& event) override {}

private:
  const evie::IInputManager* input_manager_{ nullptr };
};


class Sandbox : public evie::Application
{
public:
  [[nodiscard]] evie::Error Init()
  {
    evie::WindowProperties props;
    props.name = "Sandbox";
    props.dimensions.width = 1260;
    props.dimensions.height = 720;
    // Always Initialise the engine before doing anything with it.
    evie::Error err = Initialise(props);
    if (err.Good()) {
      t_layer_ = TestLayer(GetInputManager());
      PushLayerFront(t_layer_);
    }
    return err;
  }
  ~Sandbox() override = default;

private:
  TestLayer t_layer_;
};

std::unique_ptr<evie::Application> CreateApplication()
{
  std::unique_ptr<Sandbox> app = std::make_unique<Sandbox>();
  evie::Error err = app->Init();
  if (err.Good()) {
    return app;
  } else {
    return nullptr;
  }
}
