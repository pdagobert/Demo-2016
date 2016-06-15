#include "Water.hpp"
#include "VertexLayout.hpp"
#include "Geometry.hpp"
#include "Loader.hpp"
#include "RenderTarget.hpp"
#include "OpenGL/Shader.hpp"
#include "Scene.hpp"
#include "Application.hpp"
#include "Utility.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

Water::Water()
{
	VertexLayout layout;
	layout.addAttribute( VertexAttribute::Position, 3 );
	layout.addAttribute( VertexAttribute::TexCoords, 2 );

	m_waterPlane = Mesh( createSubdivisedPlane( 1, 1, 513.0f, layout ), Primitive::Triangles );

	Texture2DParameters params;
	params.min = TextureFilter::LinearMipMapLinear;
	params.mag = TextureFilter::Linear;
	params.s = params.t = TextureWrap::Repeat;
	params.mipmaps = true;
	params.anisotropy = 16.0f;

	for( int i = 0; i < 5; i++ )
	{
		std::string path = "Textures/Water/";

		std::string fileName = path + "DudvMap" + utility::to_string( i + 1 ) + ".png";
		m_dudvMaps.push_back( Loader::loadTexture2D( fileName, params ) );

		fileName = path + "NormalMap" + utility::to_string( i + 1 ) + ".png";
		m_normalMaps.push_back( Loader::loadTexture2D( fileName, params ) );
	}
}

void Water::initUniforms( const Shader& shader )
{
	shader.bind();
	shader.setUniform( "dudvMap", 0 );
	shader.setUniform( "normalMap", 1 );
	shader.setUniform( "depthMap", 2 );
	shader.setUniform( "reflectionMap", 3 );
	shader.setUniform( "refractionMap", 4 );
}

void Water::setUniforms( const Shader& shader, const Scene& scene, const Settings& settings ) const
{
	shader.setUniform( "viewProj", scene.getCamera().getMatrix() );
	shader.setUniform( "world", glm::translate( glm::mat4(), glm::vec3( 256.0f, 0.0f, -256.0f ) + glm::vec3( 0.0f, settings.waterHeight, 0.0f ) ) );
	shader.setUniform( "textureRepeat", settings.textureRepeat );
	shader.setUniform( "speed", settings.speed );
	shader.setUniform( "time", (float)glfwGetTime() );
	shader.setUniform( "distortionStrength", settings.distortionStrength );
	shader.setUniform( "softEdgesSize", settings.softEdgesSize );
	shader.setUniform( "fixedFresnelNormal", (int)settings.fixedFresnelNormal );
	shader.setUniform( "fresnelStrength", settings.fresnelStrength );

	shader.setUniform( "cameraPos", scene.getCamera().getPosition() );
	shader.setUniform( "nearPlane", scene.getCamera().getNear() );
	shader.setUniform( "farPlane", scene.getCamera().getFar() );

	glm::vec3 lightPos = glm::normalize( settings.lightPos ) * settings.shadowMapDist;
	lightPos += glm::vec3( 256.0f, 0.0f, -256.0f );

	glm::vec3 target = glm::vec3( 256.0f, 0.0f, -256.0f ) + glm::vec3( 1.0, 0.0, 1.0 );

	shader.setUniform( "lightDir", glm::normalize( lightPos - target ) );
}

void Water::bindTextures( const Settings& settings, const Texture2D& reflection, const Texture2D& refraction, const Texture2D& depth ) const
{
	m_dudvMaps[ settings.textureIndex ].bind( 0 );
	m_normalMaps[ settings.textureIndex ].bind( 1 );
	depth.bind( 2 );
	reflection.bind( 3 );
	refraction.bind( 4 );
}

void Water::render( RenderTarget& target ) const
{
	m_waterPlane.render( target );
}