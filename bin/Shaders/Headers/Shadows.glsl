bool isInRange( float x )
{
	return ( x >= 0.0 && x < 1.0 );
}

bool isInRange( vec3 position )
{
	return isInRange( position.x ) && isInRange( position.y ) && isInRange( position.z );
}

float getShadowFactor( vec4 lightSpacePos, sampler2D shadowMap, float bias )
{
	vec3 position = lightSpacePos.xyz / lightSpacePos.w;
	position = position * 0.5 + 0.5;

	if( !isInRange( position ) )
		return 0.0;

	float currentDepth = position.z - bias;
	float closestDepth = texture( shadowMap, position.xy ).r;

	return step( closestDepth, currentDepth );
}

float getShadowFactorPCF( vec4 lightSpacePos, sampler2D shadowMap, float bias )
{
	vec3 position = lightSpacePos.xyz / lightSpacePos.w;
	position = position * 0.5 + 0.5;

	if( !isInRange( position ) )
		return 0.0;

	float currentDepth = position.z - bias;
	
	float factor = 0.0;
	int numSamples = 3 * 3;
	vec2 texelSize = 1.0 / textureSize( shadowMap, 0 );

	for( int y = -2; y <= 2; y++ )
	{
		for( int x = -2; x <= 2; x++ )
		{
			float closestDepth = texture( shadowMap, position.xy + vec2( x, y ) * texelSize ).r;
			factor += step( closestDepth, currentDepth );
		}
	}

	return factor / numSamples;
}