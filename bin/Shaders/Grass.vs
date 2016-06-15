#version 330 core

layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec2 texCoords;
layout( location = 2 ) in mat4 world;

uniform sampler2D heightMap;

uniform mat4 viewProj;
uniform mat4 preWorld;
uniform mat4 lightViewProj;

out vec2 TexCoords;
out float Height;
out vec4 LightSpacePosition;

float getHeight( vec2 pos )
{
	float height = texture( heightMap, pos / 513.0 ).r;
	Height = height * 128.0;
	return Height;
}

void main()
{
	vec4 pos = world * preWorld * vec4( position, 1.0 );
	pos.y += getHeight( vec2( world[ 3 ][ 0 ], -world[ 3 ][ 2 ] ) );

	gl_Position = viewProj * pos;

	TexCoords = texCoords;
	LightSpacePosition = lightViewProj * pos;
}