#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;
in vec2 TexCoords;

struct Material
{
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
  sampler2D emission;
};

uniform Material material;

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

void main()
{
  // Spotlight position is 0.0,0.0,0.0 because we do everything in view space
  // Camera is at 0.0,0.0,0.0
  vec3 lightDir = normalize(vec3(0.0,0.0,0.0)-FragPos);
  // direction vector is 0.0,0.0,1.0 because we do the direction from the fragpos to the light
  float theta = dot(lightDir, vec3(0.0,0.0,1.0));
  float epsilon = light.cutOff - light.outerCutOff;
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
  // Ambient
  vec3 ambient = vec3(texture(material.diffuse, TexCoords)) * light.ambient;

  // Diffuse
  vec3 norm = normalize(Normal);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

  // Specular
  vec3 viewDir = normalize(-FragPos);
  vec3 reflectdir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectdir), 0.0), material.shininess);
  vec3 specular = (spec * vec3(texture(material.specular, TexCoords))) * light.specular;

  // Emission
  vec3 emission_scale = step(vec3(1.0), 1.0 - vec3(texture(material.specular, TexCoords)));
  vec3 emission = vec3(texture(material.emission, TexCoords)) * emission_scale * 0.0;

  // Attenuation
  float distance = length(LightPos - FragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
  attenuation = 1.0;

  diffuse *= attenuation;
  ambient *= attenuation;
  specular *= attenuation;

  diffuse *= intensity;
  specular *= intensity;

  vec3 result = ambient + diffuse + specular + emission;

  FragColor = vec4(result, 1.0);
}