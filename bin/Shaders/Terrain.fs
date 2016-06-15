#version 330 core

uniform sampler2D normalMap;

uniform sampler2D cliffMask;
uniform sampler2D flowMask;
	
uniform sampler2D cliffDiffuse;

//uniform sampler2D shadowMap;
//uniform float shadowBias;

uniform bool squareAltitude;
uniform float maxHeight;
uniform float snowHeight;
uniform float flowHeight;
uniform float flowStrength;

//uniform vec3 lightPos;
//uniform float lightIntensity;

in vec3 Position;
in vec2 TexCoords;
in vec3 Normal;
in vec4 LightSpacePosition;

out vec4 Color;

#include "Headers/Shadows.glsl"
#include "Headers/Utility.glsl"

vec3 grassColor = toLinearColor( vec3( 0.3, 0.5, 0.1 ) );
const vec3 cliffColor = vec3( 1.0 );
const vec3 snowColor = vec3( 1.0 );

vec3 getTextureColor()
{
	float factor = texture( cliffMask, TexCoords ).r;

	vec3 grass = grassColor;
	vec3 cliff = cliffColor * toLinearColor( texture( cliffDiffuse, TexCoords * 20.0f ).rgb );

	return mix( grass, cliff, factor );
}

vec3 getHeightColor()
{
	float factor = ( Position.y - snowHeight ) / ( maxHeight - snowHeight );
	factor = clamp( factor, 0.0, 1.0 );

	if( squareAltitude )
		factor *= factor;

	return snowColor * factor;
}

vec3 getFlowColor()
{
	float flow = texture( flowMask, TexCoords ).r;

	float factor = ( Position.y - flowHeight ) / ( maxHeight - flowHeight );
	factor = clamp( factor, 0.0, 1.0 );

	return snowColor * flow * flowStrength * factor;
}

float getLightingFactor()
{
	/*float intensity = lightIntensity;

	float ambiantFactor = 0.4;
	float diffuseFactor = 1.0 - ambiantFactor;

	vec3 normal = texture( normalMap, TexCoords ).rbg;
	normal = normalize( normal );*/

	return 0.7;// * ( 1.0 - getShadowFactor( LightSpacePosition, shadowMap, shadowBias ) );//( ambiantFactor + max( dot( normal, normalize( lightPos ) ), 0.0 ) * ( 1.0 - getShadowFactor(  LightSpacePosition, shadowMap, 0.005 ) ) ) * intensity;
}

void main()
{
	Color = vec4( getHeightColor(), 1.0 );

	vec3 texColor = getTextureColor();

	vec3 color = getTextureColor();
	color += getHeightColor();
	color += getFlowColor();

	color *= getLightingFactor();

	Color = vec4( color, 1.0 );
}
/*uniform sampler2D normalMap;
uniform sampler2D flowMask;
uniform sampler2D cliffMask;

uniform sampler2D grassDiffuse;
uniform sampler2D cliffDiffuse;

uniform sampler2D shadowMap;

uniform vec3 lightPos;

uniform bool squareAltitude; // false
uniform float maxHeight; // 64
uniform float snowHeight; // 30
uniform float flowStrength; // 4
uniform float flowHeight; // 25

uniform float bias;

in vec3 Position;
in vec2 TexCoords;
in vec3 Normal;
in vec4 LightSpacePosition;

out vec4 Color;


void main()
{
	vec3 color = getDiffuse(); // without snow
	color += vec3( 1.0 ) * heightMod; // snow added by altitude

	if( Position.y > flowHeight )
		color += texture( flowMask, TexCoords ).r * flowStrength; // snow added with flows
	else
		color += texture( flowMask, TexCoords ).r * flowStrength * Position.y / flowHeight; // snow added with flows

	vec3 normal = texture( normalMap, TexCoords ).rbg;
	normal = normalize( normal );

	float diffuseFactor = max( dot( normal, normalize( lightPos ) ), 0.0 );

	vec3 finalColor = color * ( 0.2 + ( 1.0 - getShadowFactor() ) * 0.8 );

	Color = vec4( finalColor, 1.0 );
}*/