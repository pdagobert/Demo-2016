#include "Objects.hpp"
#include "VertexLayout.hpp"
#include "Loader.hpp"
#include <glm/glm.hpp>
#include "RenderTarget.hpp"
#include "OpenGL/Shader.hpp"
#include "Scene.hpp"
#include "Application.hpp"

Objects::Objects( const std::string& mesh, const std::string& diffuse, const std::string& mask, const std::vector< Instance >& instances )
{
	VertexLayout layout;
	layout.addAttribute( VertexAttribute::Position, 3 );
	layout.addAttribute( VertexAttribute::TexCoords, 2 );
	layout.addAttribute( VertexAttribute::Normal, 3 );

	m_mesh = Loader::loadInstancedModel( mesh, layout, instances );

	Texture2DParameters params;
	params.min = TextureFilter::LinearMipMapLinear;
	params.mag = TextureFilter::Linear;
	params.s = params.t = TextureWrap::Repeat;
	params.mipmaps = true;

	m_diffuse = Loader::loadTexture2D( diffuse, params );
	m_mask = Loader::loadTexture2D( mask, params );
}

Objects::Objects( const std::string& mesh, int meshIndex, const std::string& diffuse, const std::string& mask, const std::vector< Instance >& instances )
{
	VertexLayout layout;
	layout.addAttribute( VertexAttribute::Position, 3 );
	layout.addAttribute( VertexAttribute::TexCoords, 2 );
	layout.addAttribute( VertexAttribute::Normal, 3 );

	m_mesh = Loader::loadInstancedModel( mesh, layout, instances, meshIndex );

	Texture2DParameters params;
	params.min = TextureFilter::LinearMipMapLinear;
	params.mag = TextureFilter::Linear;
	params.s = params.t = TextureWrap::Repeat;
	params.mipmaps = true;

	m_diffuse = Loader::loadTexture2D( diffuse, params );
	m_mask = Loader::loadTexture2D( mask, params );
}

void Objects::initUniforms( const Shader& shader )
{
	shader.bind();
	shader.setUniform( "diffuseMap", 0 );
	shader.setUniform( "mask", 1 );
	shader.setUniform( "heightMap", 2 );
	//shader.setUniform( "shadowMap", 3 );
}

void Objects::setUniforms( const Shader& shader, const Scene& scene, const Settings& settings ) const
{
	shader.setUniform( "viewProj", scene.getCamera().getMatrix() );
	shader.setUniform( "lightPos", settings.lightPos );
}

void Objects::bindTextures( const Scene& scene ) const
{
	m_diffuse.bind( 0 );
	m_mask.bind( 1 );
	scene.getHeightMap().bind( 2 );
}

void Objects::render( RenderTarget& target ) const
{
	m_mesh.render( target );
}