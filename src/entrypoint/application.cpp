#include <memory>

#include "evie/application.h"
#include "evie/error.h"
#include "evie/events.h"
#include "evie/input_manager.h"
#include "evie/logging.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "window/debug_layer.h"
#include "window/event_manager.h"
#include "window/input_manager_impl.h"
#include "window/key_events.h"
#include "window/layer_queue.h"
#include "window/mouse_events.h"
#include "window/window.h"

#include "glad/glad.h"

#include "GLFW/glfw3.h"

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

  // ------------ SHADERS ---------------
  // Vertex shader
  const char* vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "   ourColor = aColor;\n"
    "}\0";
  unsigned int vertexShader;
  // Create the shader
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  // Set the shader source code
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  // Compile the shader
  glCompileShader(vertexShader);
  // Check if compilation succeeded
  int success;
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    EV_ERROR("ERROR::SHADER::VERTEX::COMPILATION_FAILED");
    EV_ERROR("{}", infoLog);
    return;
  }

  // Fragment shader
  const char* fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(ourColor, 1.0);\n"
    "}\0";
  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  // Check if compilation succeeded
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    EV_ERROR("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED");
    EV_ERROR("{}", infoLog);
    return;
  }
  // Fragment shader_2
  const char* fragmentShaderSource_2 =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(ourColor, 1.0);\n"
    "}\0";
  unsigned int fragmentShader_2;
  fragmentShader_2 = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader_2, 1, &fragmentShaderSource_2, NULL);
  glCompileShader(fragmentShader_2);
  // Check if compilation succeeded
  glGetShaderiv(fragmentShader_2, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    EV_ERROR("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED");
    EV_ERROR("{}", infoLog);
    return;
  }

  // Create a shader program
  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // Check if linking failed
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    EV_ERROR("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED");
    EV_ERROR("{}", infoLog);
    return;
  }

  // Create a shader program
  unsigned int shaderProgram_2;
  shaderProgram_2 = glCreateProgram();
  glAttachShader(shaderProgram_2, vertexShader);
  glAttachShader(shaderProgram_2, fragmentShader_2);
  glLinkProgram(shaderProgram_2);
  // Check if linking failed
  glGetProgramiv(shaderProgram_2, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(shaderProgram_2, 512, NULL, infoLog);
    EV_ERROR("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED");
    EV_ERROR("{}", infoLog);
    return;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  glDeleteShader(fragmentShader_2);
  // ------------ SHADERS ---------------


  // --------- Vertex Data -------------
  float vertices[] = {
    -1.0f,
    -0.5f,
    0.0f,// Bottom left position
    1.0f,
    0.0f,
    0.0f,// color
    -0.5f,
    0.5f,
    0.0f,// Top middle
    0.0f,
    1.0f,
    0.0f,// color
    0.0f,
    -0.5f,
    0.0f,// Bottom right
    0.0f,
    0.0f,
    1.0f,// color
  };

  float vertices_2[] = {
    0.0f,
    -0.5f,
    0.0f,// Bottom right
    1.0f,
    0.0f,
    0.0f, // color
    0.5f,
    0.5f,
    0.0f,// Top Middle
    0.0f,
    1.0f,
    0.0f, // color
    1.0f,
    -0.5f,
    0.0f,// Bottom right
    0.0f,
    0.0f,
    1.0f,
  };

  // unsigned int indices[] = {
  //   // note that we start from 0!
  //   0,
  //   2,
  //   1,// first triangle
  //   2,
  //   4,
  //   3// second triangle
  // };

  unsigned int VAO;
  unsigned int VAO_2;
  unsigned int VBO;
  unsigned int VBO_2;
  unsigned int EBO;
  // Generate a vertex array object
  glGenVertexArrays(1, &VAO);
  glCheckError();
  glGenVertexArrays(1, &VAO_2);
  glCheckError();
  // Generate vertex buffer object buffer
  glGenBuffers(1, &VBO);
  glCheckError();
  glGenBuffers(1, &VBO_2);
  glCheckError();
  // Generate element buffer object buffer
  // glGenBuffers(1, &EBO);
  // glCheckError();
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
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // glCheckError();
  // // Add data to the buffer
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  // glCheckError();

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glCheckError();
  glEnableVertexAttribArray(0);
  glCheckError();

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
  glCheckError();
  glEnableVertexAttribArray(1);
  glCheckError();

  glBindVertexArray(VAO_2);
  glCheckError();
  glBindBuffer(GL_ARRAY_BUFFER, VBO_2);
  glCheckError();
  // Add data to the buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_2), vertices_2, GL_STATIC_DRAW);
  glCheckError();

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glCheckError();
  glEnableVertexAttribArray(0);
  glCheckError();
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
  glCheckError();
  glEnableVertexAttribArray(1);
  glCheckError();

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // Game loop
  while (running_) {
    impl_->window_->PollEvents();
    // Set the color to clear the screen with
    glClearColor(0.2F, 0.3F, 0.3F, 1.0F);
    // Clear the screen color buffer
    glClear(GL_COLOR_BUFFER_BIT);
    float timeValue = static_cast<float>(glfwGetTime());
    float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
    int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
    glUseProgram(shaderProgram);
    glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    vertexColorLocation = glGetUniformLocation(shaderProgram_2, "ourColor");
    glUseProgram(shaderProgram_2);
    glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
    glBindVertexArray(VAO_2);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    for (const auto& layer_wrapper : impl_->layer_queue_) {
      layer_wrapper.layer->OnUpdate();
    }
    impl_->window_->SwapBuffers();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteProgram(shaderProgram);
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
