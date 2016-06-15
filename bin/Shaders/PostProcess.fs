#version 330 core

uniform sampler2D frame;
uniform float brightness;

in vec2 TexCoords;

layout( location = 0 ) out vec4 Color;
layout( location = 1 ) out vec4 BrightColor;

void main()
{
	vec3 texColor = texture( frame, TexCoords ).rgb;
	Color = vec4( texColor, 1.0 );

	float currentBrightness = dot( Color.rgb, vec3( 0.2126, 0.7152, 0.0722 ) );
	if( currentBrightness > brightness )
		BrightColor = vec4( texColor, 1.0 );
	else
		BrightColor = vec4( 0.0 );
}