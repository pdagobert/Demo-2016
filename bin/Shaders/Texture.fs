#version 330 core

uniform sampler2D frame;

uniform int type;
uniform bool isOrtho;

uniform float nearPlane;
uniform float farPlane;

in vec2 TexCoords;

out vec4 Color;

#include "Headers/Utility.glsl"

void main()
{
	vec3 texColor = texture( frame, TexCoords ).rgb;

	switch( type )
	{
		case 0: // default
		case 1: // occlusion
		case 2: // radial blur
		case 4: // reflection map
		case 5: // refraction map
		case 7: // fullscene
		case 8: // brightness
		case 9: // blur
		{
			texColor = texColor;
			break;
		}

		case 3: // shadow map
		case 6: // water depth
		{
			texColor = vec3( texColor.x );
			break;
		}
	}

	Color = vec4( texColor, 1.0 );
}