#version 330 core

in vec3 col;
in vec2 tex;

out vec4 fragCol;

uniform sampler2D Texture;
uniform int isTex = 0;

void main()
{
	if(isTex == 1)
		fragCol = texture(Texture, tex);
	else
		fragCol = vec4(col, 1.f);
}
