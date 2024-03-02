#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 FragPos;
out vec3 LightPos;
out vec2 TexCoords;

struct Light
{
  vec3 position;
  vec3 direction;
  float cutOff;
  float outerCutOff;


  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
};


uniform Light light;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 lightPos;

void main()
{
  gl_Position = projection * view * model * vec4(aPos, 1.0);
  // Calculate the fragment position in view space.
  FragPos = vec3(view * model * vec4(aPos, 1.0));
  // We are required to transpose and inverse the model view matrix.
  // This is because we don't want to scale the normals along with the models view matrix scale.
  // This normal matrix should be calculated on the CPU and sent as a uniform as `inverse()` is
  // costly for the GPU.
  Normal = mat3(transpose(inverse(view * model))) * aNormal;
  // Move world space lightposition to camera view co-ordinate space.
  LightPos = vec3(view * vec4(light.position, 1.0));
  TexCoords = aTexCoords;
}