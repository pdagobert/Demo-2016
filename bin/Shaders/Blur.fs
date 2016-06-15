#version 330 core

uniform sampler2D frame;
uniform bool isHorizontal;

in vec2 TexCoords;

out vec4 Color;

const float weights[ 5 ] = float[ 5 ]( 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 );

void main()
{
	vec2 texelSize = 1.0 / textureSize( frame, 0 );

	vec3 color = texture( frame, TexCoords ).rgb * weights[ 0 ];

	if( isHorizontal )
	{
		for( int i = 1; i < 5; i++ )
		{
			vec2 offset = vec2( texelSize.x * i, 0.0 );
			color += texture( frame, TexCoords + offset ).rgb * weights[ i ];
			color += texture( frame, TexCoords - offset ).rgb * weights[ i ];
		}
	}
	else
	{
		for( int i = 1; i < 5; i++ )
		{
			vec2 offset = vec2( 0.0, texelSize.y * i );
			color += texture( frame, TexCoords + offset ).rgb * weights[ i ];
			color += texture( frame, TexCoords - offset ).rgb * weights[ i ];
		}
	}

	Color = vec4( color, 1.0 );
}