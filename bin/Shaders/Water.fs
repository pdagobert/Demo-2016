#version 330 core

uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform sampler2D reflectionMap;
uniform sampler2D refractionMap;

uniform float speed;
uniform float time;
uniform float distortionStrength;
uniform float softEdgesSize;
uniform bool fixedFresnelNormal;
uniform float fresnelStrength;

uniform float nearPlane;
uniform float farPlane;
uniform vec3 cameraPos;
uniform vec3 lightDir;

in vec4 ClipCoords;
in vec3 Position;
in vec2 TexCoords;

out vec4 Color;

#include "Headers/Utility.glsl"

vec2 getDistortion()
{
	vec2 distortion = texture( dudvMap, vec2( TexCoords + speed * time ) ).rg;

	distortion = distortion * 2.0 - 1.0;
	distortion *= distortionStrength;

	return distortion;
}

float depthToDistance( float depth )
{
	return 2.0 * nearPlane * farPlane / ( farPlane + nearPlane - ( 2.0 * depth - 1.0 ) * ( farPlane - nearPlane ) );
}

float getDepthAttenuation( vec2 texCoords )
{
	float depth = texture( depthMap, texCoords ).r;

	float underwaterDist = depthToDistance( depth );
	float waterDist = depthToDistance( gl_FragCoord.z );

	return ( underwaterDist - waterDist ) / softEdgesSize;
}

vec3 getReflectionColor( vec2 texCoords, vec2 distortion, float depthAttenuation )
{
	vec2 reflectionTexCoords = vec2( texCoords.x, -texCoords.y ) + distortion * depthAttenuation;

	reflectionTexCoords.x = clamp( reflectionTexCoords.x, 0.001, 0.999 );
	reflectionTexCoords.y = clamp( reflectionTexCoords.y, -0.999, -0.001 );

	return texture( reflectionMap, reflectionTexCoords ).rgb;
}

vec3 getRefractionColor( vec2 texCoords, vec2 distortion, float depthAttenuation )
{
	vec2 refractionTexCoords = texCoords + distortion * depthAttenuation;
	refractionTexCoords = clamp( refractionTexCoords, 0.001, 0.999 );

	return texture( refractionMap, refractionTexCoords ).rgb;
}

vec3 getNormal( vec2 distortion, float depthAttenuation )
{
	vec3 normal = texture( normalMap, TexCoords + distortion * depthAttenuation ).rbg * 2.0 - 1.0;
	normal.y *= 3.0;

	return normalize( normal );
}

float getFresnel( vec3 viewDir, vec3 normal )
{
	float fresnel = dot( viewDir, fixedFresnelNormal ? vec3( 0.0, 1.0, 0.0 ) : normal );
	fresnel = pow( fresnel, fresnelStrength );
	fresnel = clamp( fresnel, 0.0, 1.0 );

	return fresnel;
}

float getSpecularColor( vec3 viewDir, vec3 normal )
{
	vec3 lightDir = normalize( lightDir );
	vec3 halfVector = normalize( viewDir + lightDir );

	float factor = max( dot( normal, halfVector ), 0.0 );
	factor = pow( factor, 256 );

	return factor * 4.0;
}

void main()
{
	vec2 distortion = getDistortion();
	vec2 texCoords = ( ClipCoords.xy / ClipCoords.w ) / 2.0 + 0.5;
	float depthAttenuation = getDepthAttenuation( texCoords );
	//depthAttenuation = clamp( depthAttenuation, 0.0, 1.0 );
	
	vec3 reflectionColor = getReflectionColor( texCoords, distortion, depthAttenuation );
	vec3 refractionColor = getRefractionColor( texCoords, distortion, depthAttenuation );

	vec3 viewDir = normalize( cameraPos - Position );
	vec3 normal = getNormal( distortion, depthAttenuation );

	vec3 color = mix( reflectionColor, refractionColor, getFresnel( viewDir, normal ) );
	color = mix( color, toLinearColor( vec3( 0.0, 0.3, 0.5 ) ), 0.2 );
	color += getSpecularColor( viewDir, normal );// * depthAttenuation / 20;

	Color.rgb = color;
	Color.a = clamp( depthAttenuation, 0.0, 1.0 );
}