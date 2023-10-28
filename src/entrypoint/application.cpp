#include <memory>
#include <thread>

#include "evie/application.h"
#include "evie/error.h"
#include "evie/events.h"
#include "evie/ids.h"
#include "evie/input_manager.h"
#include "evie/logging.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "rendering/shader.h"
#include "rendering/shader_program.h"
#include "window/debug_layer.h"
#include "window/event_manager.h"
#include "window/input_manager_impl.h"
#include "window/key_events.h"
#include "window/layer_queue.h"
#include "window/mouse_events.h"
#include "window/window.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "glad/glad.h"

#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#if defined EVIE_PLATFORM_WINDOWS || defined EVIE_PLATFORM_APPLE || defined EVIE_PLATFORM_UNIX
#include "window/glfw_window.h"
#endif

namespace evie {
GLenum glCheckError_(const char* file, int line)
{
  GLenum errorCode;
  while ((errorCode = glGetError()) != GL_NO_ERROR) {
    std::string error;
    switch (errorCode) {
    case GL_INVALID_ENUM:
      error = "INVALID_ENUM";
      break;
    case GL_INVALID_VALUE:
      error = "INVALID_VALUE";
      break;
    case GL_INVALID_OPERATION:
      error = "INVALID_OPERATION";
      break;
    case GL_OUT_OF_MEMORY:
      error = "OUT_OF_MEMORY";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      error = "INVALID_FRAMEBUFFER_OPERATION";
      break;
    }
    std::cout << error << " | " << file << " (" << line << ")" << std::endl;
  }
  return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

// =========================
// Impl
class Application::Impl
{
public:
  void PushLayerFront(Layer& layer);
  void PushLayerBack(Layer& layer);

private:
  friend Application;
  std::unique_ptr<IWindow> window_;
  std::unique_ptr<EventManager> event_manager_;
  std::unique_ptr<Layer> debug_layer_;
  std::unique_ptr<IInputManager> input_manager_;
  LayerQueue layer_queue_;
};

void Application::Impl::PushLayerFront(Layer& layer) { layer_queue_.PushFront(layer); }
void Application::Impl::PushLayerBack(Layer& layer) { layer_queue_.PushBack(layer); }
// =========================

Application::Application() : impl_(new Impl()) {}
Application::~Application() { delete impl_; }

const IInputManager* Application::GetInputManager() const { return impl_->input_manager_.get(); }

Error Application::Initialise(const WindowProperties& props)
{
  // Logging
  LoggingManager::Init();
  EV_INFO("Logging Initialised");

  // Input
  // At the minute we only build for these platforms but a different window impementation will be needed for android
  // etc.
#if defined EVIE_PLATFORM_WINDOWS || defined EVIE_PLATFORM_APPLE || defined EVIE_PLATFORM_UNIX
  impl_->input_manager_ = std::make_unique<InputManager>();
#endif

  // Window
  // At the minute we only build for these platforms but a different window impementation will be needed for android
  // etc.
#if defined EVIE_PLATFORM_WINDOWS || defined EVIE_PLATFORM_APPLE || defined EVIE_PLATFORM_UNIX
  impl_->window_ = std::make_unique<GLFWWindow>();
  evie::Error err = impl_->window_->Initialise(props);
  impl_->debug_layer_ = std::make_unique<DebugLayer>(impl_->window_->GetNativeWindow());
  impl_->PushLayerBack(*impl_->debug_layer_);
#endif

  // Event System
  impl_->event_manager_ = std::make_unique<EventManager>(impl_->layer_queue_, impl_->input_manager_.get());
  // Register CloseWindow so that we can shut the game down.
  impl_->event_manager_->SubscribeToEventType(
    EventType::WindowClose, [this]([[maybe_unused]] Event& event) { this->CloseWindow(); });

  // TODO: Get rid of this and just construct window with the Event Manager
  if (err.Good()) {
    err = impl_->window_->RegisterEventManager(*impl_->event_manager_);
  }

  if (err.Good()) {
    initialised_ = true;
  }

  return err;
}

void Application::Run()
{
  if (!initialised_) {
    EV_ERROR("Application isn't initialised. Please run Initialise() before Run(). Exiting.");
    return;
  }
  // ---- GLM ----
  glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
  glm::mat4 trans = glm::mat4(1.0f);
  trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
  vec = trans * vec;
  EV_INFO("x: {}, y: {}, z: {}", vec.x, vec.y, vec.z);

  // ------- Textures ---------
  // set the texture wrapping/filtering options (on the currently bound texture object)
  // load and generate the texture
  int width, height, nrChannels;
  unsigned char* data =
    stbi_load("C:\\Users\\willa\\devel\\Evie\\textures\\container.jpg", &width, &height, &nrChannels, 0);
  unsigned int texture_id;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Dead texture\n";
  }
  stbi_image_free(data);
  stbi_set_flip_vertically_on_load(true);
  data = stbi_load("C:\\Users\\willa\\devel\\Evie\\textures\\awesomeface.png", &width, &height, &nrChannels, 0);
  unsigned int texture_id_2;
  glGenTextures(1, &texture_id_2);
  glBindTexture(GL_TEXTURE_2D, texture_id_2);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Dead texture\n";
  }
  stbi_image_free(data);
  // ------------ SHADERS ---------------
  VertexShader vertex_shader;
  Error err = vertex_shader.Initialise("C:\\Users\\willa\\devel\\Evie\\shaders\\vertex_shader.vs");
  glCheckError();
  FragmentShader fragment_shader;
  if (err.Good()) {
    err = fragment_shader.Initialise("C:\\Users\\willa\\devel\\Evie\\shaders\\fragment_shader.fs");
    glCheckError();
  }

  VertexShader vertex_shader_2;
  if (err.Good()) {
    err = vertex_shader_2.Initialise("C:\\Users\\willa\\devel\\Evie\\shaders\\vertex_shader.vs");
  }

  FragmentShader fragment_shader_2;
  if (err.Good()) {
    err = fragment_shader_2.Initialise("C:\\Users\\willa\\devel\\Evie\\shaders\\fragment_shader.fs");
    glCheckError();
  }

  ShaderProgram shader_program;
  if (err.Good()) {
    err = shader_program.Initialise(&vertex_shader, &fragment_shader);
    glCheckError();
  }

  ShaderProgram shader_program_2;
  if (err.Good()) {
    err = shader_program_2.Initialise(&vertex_shader_2, &fragment_shader_2);
    glCheckError();
  }

  // --------- Vertex Data -------------
  // float vertices[] = {
  //   -1.0f,
  //   -0.5f,
  //   0.0f,// Bottom left position
  //   1.0f,
  //   0.0f,
  //   0.0f,// color
  //   -0.5f,
  //   0.5f,
  //   0.0f,// Top middle
  //   0.0f,
  //   1.0f,
  //   0.0f,// color
  //   0.0f,
  //   -0.5f,
  //   0.0f,// Bottom right
  //   0.0f,
  //   0.0f,
  //   1.0f,// color
  // };

  // float vertices_2[] = {
  //   0.0f,
  //   -0.5f,
  //   0.0f,// Bottom right
  //   1.0f,
  //   0.0f,
  //   0.0f,// color
  //   0.5f,
  //   0.5f,
  //   0.0f,// Top Middle
  //   0.0f,
  //   1.0f,
  //   0.0f,// color
  //   1.0f,
  //   -0.5f,
  //   0.0f,// Bottom right
  //   0.0f,
  //   0.0f,
  //   1.0f,
  // };

  // clang-format off
//   float vertices[] = {
//     // positions          // colors           // texture coords
//      0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
//      0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
//     -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
//     -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
// };
float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};
  // clang-format on

  // float texCoords[] = {
  //   0.0f,
  //   0.0f,// lower-left corner
  //   1.0f,
  //   0.0f,// lower-right corner
  //   0.5f,
  //   1.0f// top-center corner
  // };

  unsigned int indices[] = {
    // note that we start from 0!
    0,
    1,
    3,// first triangle
    1,
    2,
    3// second triangle
  };

  unsigned int VAO;
  // unsigned int VAO_2;
  unsigned int VBO;
  // unsigned int VBO_2;
  unsigned int EBO;
  // Generate a vertex array object
  glGenVertexArrays(1, &VAO);
  glCheckError();
  // glGenVertexArrays(1, &VAO_2);
  // glCheckError();
  // Generate vertex buffer object buffer
  glGenBuffers(1, &VBO);
  glCheckError();
  // glGenBuffers(1, &VBO_2);
  // glCheckError();
  // Generate element buffer object buffer
  glGenBuffers(1, &EBO);
  glCheckError();
  // Bind the vertex array object
  glBindVertexArray(VAO);
  glCheckError();

  // Bind the buffer in the state machine
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glCheckError();
  // Add data to the buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glCheckError();

  // Bind the buffer in the state macine
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glCheckError();
  // Add data to the buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  glCheckError();

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));
  glCheckError();
  glEnableVertexAttribArray(0);
  glCheckError();

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
  glCheckError();
  glEnableVertexAttribArray(1);
  glCheckError();

  // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
  // glCheckError();
  // glEnableVertexAttribArray(2);
  // glCheckError();

  // glBindVertexArray(VAO_2);
  // glCheckError();
  // glBindBuffer(GL_ARRAY_BUFFER, VBO_2);
  // glCheckError();
  // Add data to the buffer
  // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_2), vertices_2, GL_STATIC_DRAW);
  // glCheckError();

  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0));
  // glCheckError();
  // glEnableVertexAttribArray(0);
  // glCheckError();
  // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
  // glCheckError();
  // glEnableVertexAttribArray(1);
  // glCheckError();

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_DEPTH_TEST);
  glm::vec3 cubePositions[] = { glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(2.0f, 5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f, 3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f, 2.0f, -2.5f),
    glm::vec3(1.5f, 0.2f, -1.5f),
    glm::vec3(-1.3f, 1.0f, -1.5f) };

  // Game loop
  shader_program.Use();
  shader_program.SetInt("ourTexture1", 0);
  shader_program.SetInt("ourTexture2", 1);
  float mixer{ 0.2f };
  float camera_x{ 0.0f };
  float camera_y{ 0.0f };
  float camera_z{ -3.0f };
  float camera_yaw{ 0.0f };
  float camera_pitch{ 0.0f };
  shader_program.SetFloat("mixer", mixer);
  while (running_ && err.Good()) {
    impl_->window_->PollEvents();
    // Set the color to clear the screen with
    glClearColor(0.2F, 0.3F, 0.3F, 1.0F);
    if (GetInputManager()->IsKeyPressed(KeyCode::K)) {
      mixer += 0.01f;
      if (mixer > 1.0f) {
        mixer = 1.0f;
      }
      shader_program.SetFloat("mixer", mixer);
    } else if (GetInputManager()->IsKeyPressed(KeyCode::J)) {
      mixer -= 0.01f;
      if (mixer < 0.0f) {
        mixer = 0.0f;
      }
      shader_program.SetFloat("mixer", mixer);
    }
    // Clear the screen color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_id_2);
    glBindVertexArray(VAO);
    for (unsigned int i = 0; i < 10; ++i) {
      glm::mat4 model = glm::mat4(1.0f);
      // This translates the object to the position in the world that we want it.
      model = glm::translate(model, cubePositions[i]);
      float angle = 20.0f * static_cast<float>(i);
      // This rotates the object to where we want it in the world space.
      if (i % 3 == 0) {
        model =
          glm::rotate(model, static_cast<float>(glfwGetTime()) * glm::radians(50.0f), glm::vec3(1.0f, 0.3f, 0.5f));
      } else {
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      }
      shader_program.SetMat4("model", glm::value_ptr(model));

      glm::mat4 view = glm::mat4(1.0f);
      // note that we're translating the scene in the reverse direction of where we want to move
      // This translates the camera/view to where we want it to be
      if (GetInputManager()->IsKeyPressed(KeyCode::S)) {
        camera_y += 0.05f;
      }
      if (GetInputManager()->IsKeyPressed(KeyCode::W)) {
        camera_y -= 0.05f;
      }
      if (GetInputManager()->IsKeyPressed(KeyCode::D)) {
        camera_x -= 0.05f;
      }
      if (GetInputManager()->IsKeyPressed(KeyCode::A)) {
        camera_x += 0.05f;
      }
      if(GetInputManager()->IsKeyPressed(KeyCode::Q)){
        camera_z += 0.05f;
      }
      if(GetInputManager()->IsKeyPressed(KeyCode::E)){
        camera_z -= 0.05f;
      }
      view = glm::translate(view, glm::vec3(camera_x, camera_y, camera_z));
      if (GetInputManager()->IsKeyPressed(KeyCode::Left)) {
        camera_yaw += 0.05f;
      }
      if (GetInputManager()->IsKeyPressed(KeyCode::Right)) {
        camera_yaw -= 0.05f;
      }
      view = glm::rotate(view, glm::radians(camera_yaw), glm::vec3(0.0f, 1.0f, 0.0f));
      if (GetInputManager()->IsKeyPressed(KeyCode::Up)) {
        camera_pitch += 0.05f;
      }
      if (GetInputManager()->IsKeyPressed(KeyCode::Down)) {
        camera_pitch -= 0.05f;
      }
      view = glm::rotate(view, glm::radians(camera_pitch), glm::vec3(1.0f, 0.0f, 0.0f));
      shader_program.SetMat4("view", glm::value_ptr(view));
      // This sets up the projection. What's our FoV? What's our aspect ratio?
      glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
      shader_program.SetMat4("projection", glm::value_ptr(projection));
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    for (const auto& layer_wrapper : impl_->layer_queue_) {
      layer_wrapper.layer->OnUpdate();
    }
    impl_->window_->SwapBuffers();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  // glDeleteBuffers(1, &EBO);
  shader_program.Delete();
  shader_program_2.Delete();
  if (err.Bad()) {
    EV_ERROR("{}", err.Message());
    std::this_thread::sleep_for(std::chrono::duration(std::chrono::seconds(5)));
  }
}

void Application::Shutdown()
{
  // Shutdown layers first as they'll be using contexts from the window like glfw, opengl etc.
  impl_->layer_queue_.Shutdown();
  impl_->window_->Destroy();
}

void Application::PushLayerFront(Layer& layer) { impl_->layer_queue_.PushFront(layer); }

void Application::PushLayerBack(Layer& layer) { impl_->layer_queue_.PushBack(layer); }
}// namespace evie
