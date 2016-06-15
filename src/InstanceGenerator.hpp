#ifndef __INSTANCE_GENERATOR_HPP__
#define __INSTANCE_GENERATOR_HPP__

#include "Mesh.hpp"
#include <glm/glm.hpp>
#include <vector>

struct InstanceData
{
	glm::vec3 position;
	float angle;
	float radius;
};

class InstanceGenerator
{
public:
	InstanceGenerator();

	std::vector< Instance > addObjects( int num, int iterations, float areaScale, float radius, float radius2, const std::vector< float >& densityMap, const glm::vec3& scaling );
	std::vector< Instance > generateGrassPatch( const std::vector< float >& densityMap );

private:
	std::vector< InstanceData > m_instances;
};
#endif