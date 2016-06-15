#include "InstanceGenerator.hpp"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdlib>

InstanceGenerator::InstanceGenerator()
{

}

std::vector< Instance > InstanceGenerator::addObjects( int num, int iterations, float areaScale, float radius, float radius2, const std::vector< float >& densityMap, const glm::vec3& scaling )
{
	std::vector< InstanceData > instances;

	int iter = 0;

	int textureSize = glm::sqrt( densityMap.size() );

	for( int i = 0; i < num; i++ )
	{
		float r = (float)rand() / RAND_MAX;
		float r2 = (float)rand() / RAND_MAX;

		glm::vec3 pos( r * areaScale, 0.0f, r2 * -areaScale );

		int x = r * textureSize;
		int y = textureSize - r2 * textureSize;

		float density = densityMap[ y * textureSize + x ];
		if( density < 0.1 || density < 0.5f )
		{
			iter++;
			continue;
		}

		bool add = true;

		/*for( const InstanceData& inst : m_instances )
		{
			if( glm::distance( inst.position, pos ) < inst.radius + radius )
			{
				add = false;
				break;
			}
		}*/

		if( add )
		{
			InstanceData i;
			i.position = pos;

			i.angle = (float)rand() / RAND_MAX * glm::pi< float >() * 2.0f;

			i.radius = radius;

			instances.push_back( i );
			m_instances.push_back( i );

			iter = 0;
		}
		else
		{
			iter++;
			if( iter > iterations )
				break;
		}
	}

	std::vector< Instance > result;

	for( const InstanceData& inst: instances )
	{
		glm::mat4 world;
		world = glm::translate( world, inst.position );
		world = glm::rotate( world, inst.angle, glm::vec3( 0.0f, 1.0f, 0.0f ) );
		world = glm::scale( world, scaling );

		result.push_back( Instance( world ) );
	}

	return result;
	
}

std::vector< Instance > InstanceGenerator::generateGrassPatch( const std::vector< float >& densityMap )
{
	std::vector< Instance > instances;

	int textureSize = glm::sqrt( densityMap.size() );

	float areaScale = 1.0f;

	for( int i = 0; i < textureSize; i++ )
	{
		for( int j = 0; j < textureSize; j++ )
		{
			float density = densityMap[ ( textureSize - 1 - i ) * textureSize + j ];

			for( int k = 0; k < 5 * density; k++ )
			{
				float r = (float)rand() / RAND_MAX;
				float r2 = (float)rand() / RAND_MAX;

				glm::vec3 pos( j + r, 0.0f, -( i + r ) );

				float angle = (float)rand() / RAND_MAX * glm::pi< float >() * 2.0f;

				glm::mat4 world;
				world = glm::translate( world, pos );
				world = glm::rotate( world, angle, glm::vec3( 0.0f, 1.0f, 0.0f ) );

				instances.push_back( Instance( world ) );
			}
		}
	}

	instances.push_back( glm::mat4() );

	return instances;
}