#include "Mesh.hpp"
#include "RenderTarget.hpp"

Mesh::Mesh() :
	m_vb(),
	m_ib(),
	m_va(),
	m_command()
{

}

Mesh::Mesh( Geometry g, Primitive primitive ) :
	m_vb(),
	m_ib(),
	m_va(),
	m_command( primitive, g.indices.getData().size(), 0 )
{
	m_vb.setData( g.vertices.getData(), Usage::Static );
	m_ib.setData( g.indices.getData(), Usage::Static );

	m_va.bind();

	m_vb.bind();
	g.vertices.bind();

	m_ib.bind();

	m_va.unbind();

	m_vb.unbind();
	m_ib.unbind();
}

Mesh::Mesh( Mesh&& mesh ) :
	m_vb( std::move( mesh.m_vb ) ),
	m_ib( std::move( mesh.m_ib ) ),
	m_va( std::move( mesh.m_va ) ),
	m_command( mesh.m_command )
{

}

Mesh& Mesh::operator=( Mesh&& mesh )
{
	m_vb = std::move( mesh.m_vb );
	m_ib = std::move( mesh.m_ib );
	m_va = std::move( mesh.m_va );
	m_command = mesh.m_command;

	return *this;
}

void Mesh::render( RenderTarget& target ) const
{
	m_va.bind();
	target.draw( m_command );
	m_va.unbind();
}

InstancedMesh::InstancedMesh() :
	m_vb(),
	m_ib(),
	m_va(),
	m_command()
{

}

InstancedMesh::InstancedMesh( Geometry g, Primitive primitive, const std::vector< Instance >& instances ) :
	m_vb(),
	m_ib(),
	m_instBuffer(),
	m_va(),
	m_command( primitive, g.indices.getData().size(), 0, instances.size() )
{
	m_vb.setData( g.vertices.getData(), Usage::Static );
	m_ib.setData( g.indices.getData(), Usage::Static );
	m_instBuffer.setData( instances, Usage::Static );

	m_va.bind();

	m_vb.bind();
	int start = g.vertices.bind();

	m_instBuffer.bind();
	Instance::bind( start );

	m_ib.bind();

	m_va.unbind();

	m_vb.unbind();
	m_ib.unbind();
	m_instBuffer.unbind();
}

InstancedMesh::InstancedMesh( InstancedMesh&& mesh ) :
	m_vb( std::move( mesh.m_vb ) ),
	m_ib( std::move( mesh.m_ib ) ),
	m_instBuffer( std::move( mesh.m_instBuffer ) ),
	m_va( std::move( mesh.m_va ) ),
	m_command( mesh.m_command )
{

}

InstancedMesh& InstancedMesh::operator=( InstancedMesh&& mesh )
{
	m_vb = std::move( mesh.m_vb );
	m_ib = std::move( mesh.m_ib );
	m_instBuffer = std::move( mesh.m_instBuffer );
	m_va = std::move( mesh.m_va );
	m_command = mesh.m_command;

	return *this;
}

void InstancedMesh::render( RenderTarget& target ) const
{
	m_va.bind();
	target.draw( m_command );
	m_va.unbind();
}