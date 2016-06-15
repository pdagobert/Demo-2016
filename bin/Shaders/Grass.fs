#version 330 core

uniform sampler2D diffuseMap;

uniform sampler2D shadowMap;
uniform float shadowBias;

in vec2 TexCoords;
in float Height;
in vec4 LightSpacePosition;

out vec4 Color;

#include "Headers/Utility.glsl"
#include "Headers/Shadows.glsl"

void main()
{
	vec4 tempColor = texture( diffuseMap, vec2( TexCoords.x, 1.0 - TexCoords.y ) );
	vec3 texColor = toLinearColor( tempColor.rgb );

	if( tempColor.a < 0.5 )
		discard;

	vec3 color = texColor;
	color *= max( 0.1, TexCoords.y * TexCoords.y ); // ambient occlusion

	color = color * 0.2 + color * 0.8 * ( 1.0 - getShadowFactor( LightSpacePosition, shadowMap, shadowBias ) );

	Color = vec4( color, 1.0 );
}