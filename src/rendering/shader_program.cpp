#include "evie/shader_program.h"
#include "evie/error.h"
#include "evie/ids.h"
#include "evie/logging.h"
#include "evie/shader.h"
#include "rendering/debug.h"

#include <array>
#include <evie/types.h>

namespace evie {

Error ShaderProgram::Initialise(VertexShader* vertex_shader, FragmentShader* fragment_shader)
{
  vertex_shader_ = vertex_shader;
  fragment_shader_ = fragment_shader;
  id_ = glCreateProgram();

  // Attach the vertex shader
  Result<ShaderID> vertex_id = vertex_shader_->GetID();
  if (vertex_id.Good()) {
    CallOpenGL(glAttachShader, id_, vertex_id->Get());
  } else {
    return vertex_id.Error();
  }

  // Attach the fragement shader
  Result<ShaderID> fragment_id = fragment_shader_->GetID();
  if (fragment_id.Good()) {
    CallOpenGL(glAttachShader, id_, fragment_id->Get());
  } else {
    return fragment_id.Error();
  }

  // Link the shaders to the program
  CallOpenGL(glLinkProgram, id_);
  int success{ 0 };
  CallOpenGL(glGetProgramiv, id_, GL_LINK_STATUS, &success);
  if (success == 0) {
    const int log_length = 512;
    std::array<char, log_length> infoLog{};
    CallOpenGL(glGetProgramInfoLog, id_, log_length, nullptr, infoLog.data());
    EV_ERROR("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED");
    EV_ERROR("{}", infoLog.data());
    return Error{ "Failed to link shader program" };
  }

  CallOpenGL(glDeleteShader, vertex_id->Get());
  CallOpenGL(glDeleteShader, fragment_id->Get());

  initialised_ = true;
  return Error::OK();
}

void ShaderProgram::Use() const { CallOpenGL(glUseProgram, id_); }

void ShaderProgram::SetBool(const std::string& name, bool value) const
{
  CallOpenGL(glUniform1i, glGetUniformLocation(id_, name.c_str()), static_cast<int>(value));
}

void ShaderProgram::SetInt(const std::string& name, int value) const
{
  CallOpenGL(glUniform1i, glGetUniformLocation(id_, name.c_str()), value);
}

void ShaderProgram::SetFloat(const std::string& name, float value) const
{
  CallOpenGL(glUniform1f, glGetUniformLocation(id_, name.c_str()), value);
}

void ShaderProgram::SetMat4(const std::string& name, float* first_element) const
{
  CallOpenGL(glUniformMatrix4fv, glGetUniformLocation(id_, name.c_str()), 1, false, first_element);
}

void ShaderProgram::SetVec3(const std::string& name, evie::vec3 vector) const
{
  CallOpenGL(glUniform3f, glGetUniformLocation(id_, name.c_str()), vector.x, vector.y, vector.z);
}

void ShaderProgram::Destroy() const { CallOpenGL(glDeleteProgram, id_); }

}// namespace evie