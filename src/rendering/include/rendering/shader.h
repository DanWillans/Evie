#ifndef EVIE_RENDERING_SHADER_H_
#define EVIE_RENDERING_SHADER_H_

#include "glad/glad.h"

#include "evie/error.h"
#include "evie/ids.h"
#include "evie/logging.h"
#include "evie/result.h"
#include "rendering/shader.h"

#include <array>
#include <filesystem>
#include <fstream>
#include <string>


namespace evie {

template<int T> class EVIE_API Shader
{
public:
  Error Initialise(const std::string& shaderFilePath)
  {
    Result<std::string> shader_code = OpenFile(shaderFilePath);
    if (shader_code.Good()) {
      id_ = glCreateShader(shader_type);
      const char* const_shader_code = shader_code->c_str();
      glShaderSource(id_, 1, &const_shader_code, nullptr);
      glCompileShader(id_);
      int success = 0;
      glGetShaderiv(id_, GL_COMPILE_STATUS, &success);
      if (success == 0) {
        const int log_length = 512;
        std::array<char, log_length> infoLog{};
        glGetShaderInfoLog(id_, log_length, nullptr, infoLog.data());
        EV_ERROR("ERROR::SHADER::COMPILATION_FAILED");
        EV_ERROR("{}", infoLog.data());
        return Error("Vertex shader failed to compile");
      }
      initialised_ = true;
      return Error::OK();
    }
    return shader_code.Error();
  }

  [[nodiscard]] Result<ShaderID> GetID() const
  {
    // Suppress this check because it makes no sense.
    // cppcheck-suppress identicalConditionAfterEarlyExit
    if (Error err = CheckIfInitialised(); err) {
      return ShaderID(id_);
    } else {
      // cppcheck-suppress identicalConditionAfterEarlyExit
      return err;
    }
  }

  [[nodiscard]] Error CheckIfInitialised() const
  {
    Error err = Error::OK();
    if (!initialised_) {
      err = Error("Shader isn't initialised. Please initialise before using.");
    }
    return err;
  }

  static constexpr int shader_type = T;

private:
  static Result<std::string> OpenFile(const std::string& file_path)
  {
    std::fstream file_stream;
    if (std::filesystem::exists(file_path)) {
      file_stream.open(file_path);
      if (file_stream.is_open()) {
        std::stringstream shader_string_stream;
        shader_string_stream << file_stream.rdbuf();
        file_stream.close();
        return shader_string_stream.str();
      } else {
        return Error{ "Error opening file" };
      }
    } else {
      return Error{ "File doesn't exist" };
    }
  }
  bool initialised_{ false };
  unsigned int id_{ 0 };
};

using VertexShader = Shader<GL_VERTEX_SHADER>;
using FragmentShader = Shader<GL_FRAGMENT_SHADER>;

}// namespace evie

#endif// !EVIE_RENDERING_SHADER_H_