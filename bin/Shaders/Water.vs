#version 330 core

layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec2 texCoords;

uniform mat4 viewProj;
uniform mat4 world;

uniform float textureRepeat;

out vec4 ClipCoords;
out vec3 Position;
out vec2 TexCoords;

void main()
{
	vec4 pos = world * vec4( position, 1.0 );
	vec4 clipCoords = viewProj * pos;
	
	gl_Position = clipCoords;

	ClipCoords = clipCoords;
	Position = pos.xyz;
	TexCoords = texCoords * textureRepeat;
}
