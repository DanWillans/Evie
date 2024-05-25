#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;
uniform float mixer;
void main()
{
   // FragColor = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, vec2(1 - TexCoord.x, TexCoord.y)), mixer);
   FragColor = texture(ourTexture1, TexCoord);
   // FragColor = vec4(1.0f, 0.5f, 0.5f, 1.0f);
}