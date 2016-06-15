#version 330 core

uniform sampler2D mask;

in vec2 TexCoords;

//out vec4 Color;

void main()
{
	float alpha = texture( mask, TexCoords ).r;
	if( alpha < 0.5 )
		discard;
}