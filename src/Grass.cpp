#include "Grass.hpp"
#include "RenderTarget.hpp"
#include "OpenGL/Shader.hpp"
#include "Scene.hpp"
#include "Application.hpp"
#include "Loader.hpp"
#include "VertexLayout.hpp"
#include "Geometry.hpp"
#include "Utility.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdlib>

Grass::Grass( const std::vector< Instance >& instances )
{
	VertexLayout layout;
	layout.addAttribute( VertexAttribute::Position, 3 );
	layout.addAttribute( VertexAttribute::TexCoords, 2 );

	m_mesh = InstancedMesh( createSubdivisedPlane( 2, 1, 1.0f, layout, false ), Primitive::Triangles, instances );

	Texture2DParameters params;
	params.min = TextureFilter::LinearMipMapLinear;
	params.mag = TextureFilter::Linear;
	params.s = params.t = TextureWrap::Repeat;
	params.mipmaps = true;

	for( int i = 0; i < 3; i++ )
	{
		std::string fileName = "Grass" + utility::to_string( i + 1 ) + ".png";

		m_diffuseMaps.push_back( Loader::loadTexture2D( "Textures/" + fileName, params ) );
	}
}

void Grass::initUniforms( const Shader& shader )
{
	shader.bind();
	shader.setUniform( "diffuseMap", 0 );
	shader.setUniform( "heightMap", 1 );
	shader.setUniform( "shadowMap", 2 );
}

void Grass::setUniforms( const Shader& shader, const Scene& scene, const Settings& settings ) const
{
	shader.setUniform( "viewProj", scene.getCamera().getMatrix() );
	shader.setUniform( "preWorld", glm::rotate( glm::mat4(), glm::radians( 90.0f ), glm::vec3( 1.0f, 0.0f, 0.0f ) ) );
}

void Grass::bindTextures( const Scene& scene, const Settings& settings ) const
{
	m_diffuseMaps[ settings.grassTexture ].bind( 0 );
	scene.getHeightMap().bind( 1 );
}

void Grass::render( RenderTarget& target ) const
{
	m_mesh.render( target );
}