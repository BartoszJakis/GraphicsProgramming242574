

#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;
uniform vec4 fractalColor;

void main()
{

    FragColor = mix(texture(texture1, TexCoord), fractalColor, 0.3);
}

