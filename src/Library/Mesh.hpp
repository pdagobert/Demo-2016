#ifndef __MESH_HPP__
#define __MESH_HPP__

#include "Geometry.hpp"
#include "OpenGL/Buffer.hpp"
#include "OpenGL/VertexArray.hpp"
#include "OpenGL/RenderCommand.hpp"

class RenderTarget;

class Mesh
{
public:
	Mesh();
	Mesh( Geometry g, Primitive primitive );

	Mesh( Mesh&& mesh );
	Mesh& operator=( Mesh&& mesh );

	Mesh( const Mesh& mesh ) = delete;
	Mesh& operator=( const Mesh& mesh ) = delete;

	void render( RenderTarget& target ) const;

private:
	VertexBuffer m_vb;
	IndexBuffer m_ib;

	VertexArray m_va;
	RenderCommand m_command;
};

struct Instance
{
	Instance( const glm::mat4& world ) :
		world( world )
	{

	}

	static void bind( int start )
	{
		glEnableVertexAttribArray( start );
		glVertexAttribPointer( start, 4, GL_FLOAT, GL_FALSE, sizeof( Instance ), reinterpret_cast< GLvoid* >( 0 ) );
		glVertexAttribDivisor( start, 1 );

		glEnableVertexAttribArray( start + 1 );
		glVertexAttribPointer( start + 1, 4, GL_FLOAT, GL_FALSE, sizeof( Instance ), reinterpret_cast< GLvoid* >( sizeof( glm::vec4 ) ) );
		glVertexAttribDivisor( start + 1, 1 );

		glEnableVertexAttribArray( start + 2 );
		glVertexAttribPointer( start + 2, 4, GL_FLOAT, GL_FALSE, sizeof( Instance ), reinterpret_cast< GLvoid* >( sizeof( glm::vec4 ) * 2 ) );
		glVertexAttribDivisor( start + 2, 1 );

		glEnableVertexAttribArray( start + 3 );
		glVertexAttribPointer( start + 3, 4, GL_FLOAT, GL_FALSE, sizeof( Instance ), reinterpret_cast< GLvoid* >( sizeof( glm::vec4 ) * 3 ) );		
		glVertexAttribDivisor( start + 3, 1 );
	}

	glm::mat4 world;
};

class InstancedMesh
{
public:
	InstancedMesh();
	InstancedMesh( Geometry g, Primitive primitive, const std::vector< Instance >& instances );

	InstancedMesh( InstancedMesh&& mesh );
	InstancedMesh& operator=( InstancedMesh&& mesh );

	InstancedMesh( const InstancedMesh& mesh ) = delete;
	InstancedMesh& operator=( const InstancedMesh& mesh ) = delete;

	void render( RenderTarget& target ) const;

private:
	VertexBuffer m_vb;
	IndexBuffer m_ib;
	InstanceBuffer m_instBuffer;

	VertexArray m_va;
	RenderCommand m_command;
};
#endif