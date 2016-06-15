#include "Skybox.hpp"
#include "RenderTarget.hpp"
#include "OpenGL/Shader.hpp"
#include "Scene.hpp"
#include "Application.hpp"
#include "Loader.hpp"
#include "VertexLayout.hpp"
#include "Geometry.hpp"

Skybox::Skybox( const std::string& path, const std::string& ext ) :
	m_cubeMap( Loader::loadCubeMap( path, ext ) )
{
	CubeMapParameters params;
	params.min = params.mag = TextureFilter::Linear;
	params.s = params.t = params.r = TextureWrap::Clamp;

	m_cubeMap.setParameters( params );

	VertexLayout layout;
	layout.addAttribute( VertexAttribute::Position, 3 );

	m_cube = Mesh( createBox( glm::vec3( 1.0f ), layout ), Primitive::Triangles );
}

void Skybox::initUniforms( const Shader& shader )
{
	shader.bind();
	shader.setUniform( "cubeMap", 0 );
}

void Skybox::setUniforms( const Shader& shader, const Scene& scene ) const
{
	shader.setUniform( "viewProj", scene.getCamera().getProjectionMatrix() * glm::mat4( glm::mat3( scene.getCamera().getViewMatrix() ) ) );
}

void Skybox::bindTextures() const
{
	m_cubeMap.bind( 0 );
}

void Skybox::render( RenderTarget& target ) const
{
	m_cube.render( target );
}