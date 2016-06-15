#version 330 core

uniform sampler2D diffuseMap;
uniform sampler2D mask;
uniform sampler2D shadowMap;

uniform vec3 lightPos;

in vec2 TexCoords;
in vec3 Normal;

out vec4 Color;

#include "Headers/Utility.glsl"

vec3 getLighting()
{
	float ambiantFactor = 0.4 * max( dot( lightPos, vec3( 0.0, 1.0, 0.0 ) ), 0.0 );
	float diffuseFactor = 1.0 - ambiantFactor;

	vec3 normal = normalize( Normal );

	vec3 lightDir = normalize( lightPos );

	float diffuse = max( dot( normal, lightDir ), 0.0 );

	return vec3( ambiantFactor + diffuse * 1.0 );
}

void main()
{
	float alpha = texture( mask, TexCoords ).r;
	if( alpha < 0.5 )
		discard;

	Color = vec4( 0.0 );
}