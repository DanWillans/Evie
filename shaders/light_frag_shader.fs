#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;

struct Material {
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
   float shininess;
};

uniform Material material;

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

void main()
{
   // Ambient
   vec3 ambient = material.ambient * light.ambient;

   // Diffuse
   vec3 norm = normalize(Normal);
   vec3 lightDir = normalize(LightPos - FragPos);
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = (diff * material.diffuse) * light.diffuse;

   // Specular
   vec3 viewDir = normalize(-FragPos);
   vec3 reflectdir = reflect(-lightDir, norm);
   float spec = pow(max(dot(viewDir, reflectdir), 0.0), material.shininess);
   vec3 specular = (spec * material.specular) * light.specular;

   vec3 result = ambient + diffuse + specular;

   FragColor = vec4(result, 1.0);
}