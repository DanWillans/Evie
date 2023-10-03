#include "rendering/shader_program.h"
#include "evie/error.h"
#include "evie/ids.h"
#include "evie/logging.h"
#include "rendering/shader.h"

#include <array>

namespace evie {
namespace {
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
}// namespace
Error ShaderProgram::Initialise(VertexShader* vertex_shader, FragmentShader* fragment_shader)
{
  Error err = Error::OK();
  vertex_shader_ = vertex_shader;
  fragment_shader_ = fragment_shader;
  id_ = glCreateProgram();
  glCheckError();
  ShaderID vertex_shader_id{ 0 };
  ShaderID fragment_shader_id{ 0 };
  if (err.Good()) {
    if (err = vertex_shader_->GetID(vertex_shader_id); err) {
      glAttachShader(id_, vertex_shader_id.Get());
      glCheckError();
    }
  }
  if (err.Good()) {
    if (err = fragment_shader->GetID(fragment_shader_id); err) {
      glAttachShader(id_, fragment_shader_id.Get());
      glCheckError();
    }
  }
  if (err.Good()) {
    glLinkProgram(id_);
    glCheckError();
    int success{ 0 };
    glGetProgramiv(id_, GL_LINK_STATUS, &success);
    glCheckError();
    if (success == 0) {
      const int log_length = 512;
      std::array<char, log_length> infoLog{};
      glGetProgramInfoLog(id_, log_length, nullptr, infoLog.data());
      glCheckError();
      EV_ERROR("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED");
      EV_ERROR("{}", infoLog.data());
      err = Error("Failed to link shader program");
    }
  }
  // Should we always delete the shader program even if there's not an error.
  if (err.Good()) {
    glDeleteShader(vertex_shader_id.Get());
    glCheckError();
    glDeleteShader(fragment_shader_id.Get());
    glCheckError();
  }

  if(err.Good()){
    initialised_ = true;
  }
  return err;
}

void ShaderProgram::Use() const { glUseProgram(id_); }

void ShaderProgram::SetBool(const std::string& name, bool value) const
{
  glUniform1i(glGetUniformLocation(id_, name.c_str()), static_cast<int>(value));
}

void ShaderProgram::SetInt(const std::string& name, int value) const
{
  glUniform1i(glad_glGetUniformLocation(id_, name.c_str()), value);
}

void ShaderProgram::SetFloat(const std::string& name, float value) const
{
  glUniform1f(glad_glGetUniformLocation(id_, name.c_str()), value);
}

void ShaderProgram::Delete() const { glDeleteProgram(id_); }

}// namespace evie