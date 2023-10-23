#ifndef EVIE_RENDERING_SHADER_PROGRAM_H_
#define EVIE_RENDERING_SHADER_PROGRAM_H_

#include "evie/error.h"
#include "evie/ids.h"
#include "rendering/shader.h"


namespace evie {
class EVIE_API ShaderProgram
{
public:
  Error Initialise(VertexShader* vertex_shader, FragmentShader* fragment_shader);
  void Use() const;
  void SetBool(const std::string& name, bool value) const;
  void SetInt(const std::string& name, int value) const;
  void SetFloat(const std::string& name, float value) const;
  void Delete() const;
  Result<ShaderProgramID> GetID()
  {
    Error err = CheckIfInitialised();
    if (err) {
      return ShaderProgramID(id_);
    }
    // cppcheck-suppress identicalConditionAfterEarlyExit
    return err;
  }

  [[nodiscard]] Error CheckIfInitialised() const
  {
    Error err = Error::OK();
    if (!initialised_) {
      err = Error("Shader Program isn't initialised. Please initialise before using.");
    }
    return err;
  }

private:
  VertexShader* vertex_shader_{ nullptr };
  FragmentShader* fragment_shader_{ nullptr };
  bool initialised_{ false };
  unsigned int id_{ 0 };
};
}// namespace evie

#endif// !EVIE_RENDERING_SHADER_PRGOAM_H_