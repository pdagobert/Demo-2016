#version 330 core

uniform sampler2D frame;
uniform vec3 ndcLightPos;

uniform int samples;
uniform float intensity;
uniform float decay;
uniform float exposure;

in vec2 TexCoords;

out vec4 Color;

#include "Headers/Shadows.glsl"

void main()
{
	/*int numSamples = 100;
	float intensity = 0.125;
	float decay = 0.96875;*/

	if( !isInRange( ndcLightPos ) )
	{
		Color = vec4( 0.0, 0.0, 0.0, 1.0 );
		return;
	}

	float inten = intensity;
	
	vec2 texCoords = TexCoords;
	vec2 dir = ndcLightPos.xy - texCoords;
	
	dir /= samples;
	
	vec3 color = texture( frame, texCoords ).rgb;
	
	for( int i = 0; i < samples; i++ )
	{
		color += texture( frame, texCoords ).rgb * inten;
		inten *= decay;
		texCoords += dir;
	}

	float smoothFactor = 1.0f;

	float x = min( ndcLightPos.x, 1.0 - ndcLightPos.x );
	float y = min( ndcLightPos.y, 1.0 - ndcLightPos.y );

	float val = min( x, y );

	if( val < 0.2 )
		smoothFactor = val / 0.2;

	Color = vec4( color * exposure * smoothFactor, 1.0 );
}