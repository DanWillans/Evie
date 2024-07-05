#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D Texture1;
uniform float mixer;
void main()
{
   FragColor = texture(Texture1, TexCoord);
}