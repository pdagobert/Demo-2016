#ifndef __GEOMETRY_HPP__
#define __GEOMETRY_HPP__

#include "VertexBufferData.hpp"
#include "IndexBufferData.hpp"
#include "VertexLayout.hpp"
#include <functional>

struct Geometry
{
	Geometry( VertexBufferData vertices, IndexBufferData indices ) :
		vertices( vertices ),
		indices( indices )
	{

	}

	VertexBufferData vertices;
	IndexBufferData indices;
};

Geometry createUVSphere( float radius, int precision, const VertexLayout& layout );

Geometry createBox( const glm::vec3& midSize, const VertexLayout& layout );

float defaultHeightFunc( int, int );

Geometry createSubdivisedPlane( int width, int height, float size, const VertexLayout& layout, bool centered = true, std::function< float( int, int ) > heightFunc = &defaultHeightFunc );

Geometry createQuad( const glm::vec2& size, const VertexLayout& layout );
Geometry createUnitQuad( const VertexLayout& layout );

void computeSmoothNormals( VertexBufferData& vertices, const IndexBufferData& indices );
#endif