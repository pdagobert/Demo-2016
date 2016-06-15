#version 330 core

uniform sampler2D frame;
uniform sampler2D godrays;
uniform sampler2D blurredFrame;

uniform bool bloomEnabled;
uniform bool toneMappingEnabled;
uniform float intensity;

uniform float exposure;

in vec2 TexCoords;

out vec4 Color;

#include "Headers/Utility.glsl"

vec3 uncharted2ToneMapping( vec3 color )
{
	float a = 0.15;
	float b = 0.50;
	float c = 0.10;
	float d = 0.20;
	float e = 0.02;
	float f = 0.30;

	return ( ( color * ( color * a + b * c ) + d * e ) / ( color * ( color * a + b ) + d * f ) ) - e / f;
}

vec3 reinhardToneMapping( vec3 color )
{
	return vec3( 1.0 ) - exp( -color );
}

void main()
{
	vec3 color = texture( frame, TexCoords ).rgb;

	color += texture( godrays, TexCoords ).rgb;

	if( bloomEnabled )
		color += texture( blurredFrame, TexCoords ).rgb;

	if( toneMappingEnabled )
		color = reinhardToneMapping( color * exposure ); //uncharted2ToneMapping( color * exposure );//vec3( 1.0 ) - exp( -color * exposure );

	color *= intensity;

	Color = vec4( toSRGBColor( color ), 1.0 );
}