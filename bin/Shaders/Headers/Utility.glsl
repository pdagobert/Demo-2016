const float gamma = 2.2;

vec3 toLinearColor( vec3 color )
{
	return pow( color, vec3( gamma ) );
}

vec3 toSRGBColor( vec3 color )
{
	return pow( color, vec3( 1.0 / gamma ) );
}

float depthToDistance( float depth, float near, float far )
{
	return 2.0 * near * far / ( far + near - ( 2.0 * depth - 1.0 ) * ( far - near ) );
}

float normalizeDepth( float depth, float near, float far )
{
	return depthToDistance( depth, near, far ) / ( far - near );
}