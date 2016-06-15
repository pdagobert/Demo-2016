#version 330 core

layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec2 texCoords;
layout( location = 2 ) in vec3 normal;
layout( location = 3 ) in mat4 world;

uniform mat4 lightViewProj;

uniform sampler2D heightMap;

out vec2 TexCoords;

float getHeight( vec2 pos )
{
	float height = texture( heightMap, pos / 513.0 ).r;
	return height * 128.0;
}

void main()
{
	vec4 pos = world * vec4( position, 1.0 );
	pos.y += getHeight( vec2( world[ 3 ][ 0 ], -world[ 3 ][ 2 ] ) );

	gl_Position = lightViewProj * pos;
	TexCoords = texCoords;
}