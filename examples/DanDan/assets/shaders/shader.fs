#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;
uniform float mixer;
void main()
{
   FragColor = texture(ourTexture1, TexCoord);
}