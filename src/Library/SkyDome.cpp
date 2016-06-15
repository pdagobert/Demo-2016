#include "SkyDome.hpp"
#include "Loader.hpp"
#include "Geometry.hpp"
#include "VertexLayout.hpp"
#include "OpenGL/RenderCommand.hpp"
#include "OpenGL/Camera.hpp"

SkyDome::SkyDome( const std::string& fileName, const Shader& shader ) :
	m_shader( shader )
{
	Texture2DParameters params;
	params.min = TextureFilter::Linear;
	params.mag = TextureFilter::Linear;
	params.s = params.t = TextureWrap::Clamp;
	params.mipmaps = false;
	params.anisotropy = 0.0f;

	m_tex = Loader::loadTexture2D( fileName, params );

	VertexLayout layout;
	layout.addAttribute( VertexAttribute::Position, 3 );
	layout.addAttribute( VertexAttribute::TexCoords, 2 );

	m_sphere = Mesh( createUVSphere( 1.0f, 50, layout ), Primitive::Triangles );
}

void SkyDome::render( RenderTarget& target, const Camera& camera )
{
	m_shader.bind();
	m_shader.setUniform( "wvp", camera.getProjectionMatrix() * glm::mat4( glm::mat3( camera.getViewMatrix() ) ) );
	m_shader.setUniform( "tex", 0 );

	m_tex.bind( 0 );

	m_sphere.render( target );
}

void SkyDome::bind( int unit ) const
{
	m_tex.bind( unit );
}