#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;
in vec2 TexCoords;
uniform mat4 view;
uniform mat4 inverse_transpose_view;

struct Material
{
  sampler2D texture_diffuse1;
  sampler2D texture_diffuse2;
  sampler2D texture_diffuse3;
  sampler2D texture_specular1;
  sampler2D texture_specular2;
  sampler2D texture_specular3;
  float shininess;
  sampler2D emission;
};

uniform Material material;

struct DirectionalLight
{
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

uniform DirectionalLight directional_light;

struct PointLight
{
  vec3 position;

  float constant;
  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};
#define NR_POINT_LIGHTS 4
uniform PointLight point_lights[NR_POINT_LIGHTS];

struct SpotLight
{
  vec3 position;
  vec3 direction;
  float cut_off;
  float outer_cut_off;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
  sampler2D image;
};

uniform SpotLight spot_light;

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_dir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);

void main()
{
  // properties
  vec3 norm = normalize(Normal);
  // We use vec3(0.0,0.0,0.0) because all our calculations are done in the view space and the camera positins is
  // 0.0,0.0,0.0.
  vec3 view_dir = normalize(vec3(0.0, 0.0, 0.0) - FragPos);

  // phase 1: Directional lighting
  vec3 result = CalcDirectionalLight(directional_light, norm, view_dir);
  // phase 2: Point lights
  // for (int i = 0; i < NR_POINT_LIGHTS; i++)
    // result += CalcPointLight(point_lights[i], norm, FragPos, view_dir);
  // phase 3: Spot light
  // result += CalcSpotLight(spot_light, norm, FragPos, view_dir);

  FragColor = vec4(result, 1.0);
}

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_dir)
{
  // Normalise the light direction vector
  // We don't need FragPos here because we already have the light direction.
  // Needs to rotate the direction vector that's in worldspace to view space.
  vec3 light_dir = normalize(vec3(inverse_transpose_view * vec4(-light.direction, 1.0)));

  // Calculate the diffuse contribution by calculating the dot product of the light direction and the normal to the
  // surface. The dot product gives a value between 0 and 1 of two vectors. The smaller the angle between the two
  // vectors the large the dot product value. This gives an "intensity" factor for the diffused light.
  float diff = max(dot(normal, light_dir), 0.0);

  // Specular shading
  // Use the reflect function to calculate the reflection direction vector.
  // \ | /
  //  \|/
  // -----
  vec3 reflect_dir = reflect(-light_dir, normal);
  // Calculate the specular contribution by calculating the dot product of the viewing direction vector and the
  // reflection direction vector. The dot product gives a value between 0 and 1 of two vectors. The smaller the angle
  // between the two vectors the large the dot product value. This gives an "intensity" factor for the specular light.
  // Take this value to the power of material.shininess to get the total specular contribution.
  float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

  // Combine results
  vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
  vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
  return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir)
{
  // Calculate the light direction vector from the fragment position to the light direction
  vec3 light_position = vec3(view * vec4(light.position, 1.0));
  vec3 light_dir = normalize(light_position - frag_pos);


  // Calculate the diffuse contribution by calculating the dot product of the light direction and the normal to the
  // surface. The dot product gives a value between 0 and 1 of two vectors. The smaller the angle between the two
  // vectors the large the dot product value. This gives an "intensity" factor for the diffused light.
  float diff = max(dot(normal, light_dir), 0.0);

  // Specular shading
  // Use the reflect function to calculate the reflection direction vector.
  // \ | /
  //  \|/
  // -----
  vec3 reflect_dir = reflect(-light_dir, normal);
  // Calculate the specular contribution by calculating the dot product of the viewing direction vector and the
  // reflection direction vector. The dot product gives a value between 0 and 1 of two vectors. The smaller the angle
  // between the two vectors the large the dot product value. This gives an "intensity" factor for the specular light.
  // Take this value to the power of material.shininess to get the total specular contribution.
  float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

  // Attenuation
  float distance = length(light_position - frag_pos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

  // Combine results
  vec3 ambient = attenuation * light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
  vec3 diffuse = attenuation * light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
  vec3 specular = attenuation * light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
  return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir)
{
  // Translate the light.position into the view space
  vec3 light_position = vec3(view * vec4(light.position, 1.0));
  // Calculate the light direction vector from the fragment position and the light position
  vec3 light_dir = normalize(light_position - frag_pos);

  // Translate the global light direction into the view space.
  vec3 light_direction = normalize(vec3(inverse_transpose_view * vec4(-light.direction, 1.0)));
  // Calculate the angle between the lights actual direction and the frag pos to light vector
  float theta = dot(light_dir, light_direction);
  // Calculate the angle difference between the the inner and outer cutoff circle
  float epsilon = light.cut_off - light.outer_cut_off;
  // Calculate the intensity of the light at theta. Dividing by epsilon allows the gradient between inner and outer
  // cutoff circle. Clamp the value between 0 and 1 to make sure anything inside the inner circle is only set to 1.
  float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);

  // Diffuse
  float diff = max(dot(normal, light_dir), 0.0);

  // Specular
  vec3 reflect_dir = reflect(-light_dir, normal);
  float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

  // Attenuation
  float distance = length(light_position - frag_pos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

  // Combine results
  vec3 ambient = intensity * attenuation * light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
  vec3 diffuse = intensity * attenuation * light.diffuse * diff * vec3(texture(light.image, TexCoords));
  vec3 specular = intensity * attenuation * light.specular * spec * vec3(texture(light.image, TexCoords));

  return (ambient + diffuse + specular);
}