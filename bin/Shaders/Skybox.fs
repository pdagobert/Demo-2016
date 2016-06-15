#version 330 core

uniform samplerCube cubeMap;

in vec3 TexCoords;

out vec4 Color;

#include "Headers/Utility.glsl"

void main()
{
	vec3 texColor = toLinearColor( texture( cubeMap, TexCoords ).rgb );
	Color = vec4( texColor, 1.0 );
}