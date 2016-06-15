#version 330 core

layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec2 texCoords;
layout( location = 2 ) in vec3 normal;

uniform mat4 viewProj;
uniform mat4 world;
uniform vec4 clip;
uniform mat4 lightViewProj;

out vec3 Position;
out vec2 TexCoords;
out vec3 Normal;
out vec4 LightSpacePosition;

void main()
{
	vec4 pos = world * vec4( position, 1.0 );

	gl_ClipDistance[ 0 ] = dot( pos, clip );
	
	gl_Position = viewProj * pos;

	Position = pos.xyz;
	TexCoords = texCoords;
	Normal = mat3( transpose( inverse( world ) ) ) * normal;
	LightSpacePosition = lightViewProj * pos;
}