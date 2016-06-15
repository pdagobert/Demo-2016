#include "Terrain.hpp"
#include "VertexLayout.hpp"
#include "Loader.hpp"
#include "RenderTarget.hpp"
#include "OpenGL/Shader.hpp"
#include "Scene.hpp"
#include "Application.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

Terrain::Terrain()
{
	VertexLayout layout;
	layout.addAttribute( VertexAttribute::Position, 3 );
	layout.addAttribute( VertexAttribute::TexCoords, 2 );
	layout.addAttribute( VertexAttribute::Normal, 3 );

	float heightScale = 128.0f;
	float meshScale = 1.0f;
	int size = 513;

	std::vector< float > heightMap = Loader::loadRawHeightmap( "Terrain/HeightMap.r32", size );

	m_mesh = Mesh( createSubdivisedPlane( size - 1, size - 1, meshScale, layout, false,
		[ &heightMap, size, heightScale ]( int x, int y )
		{
			//return 0.0f;
			return heightMap[ y * size + x ] * heightScale;
		} ), Primitive::Triangles );

	Texture2DParameters params;
	params.min = TextureFilter::LinearMipMapLinear;
	params.mag = TextureFilter::Linear;
	params.s = params.t = TextureWrap::Clamp;
	params.mipmaps = true;
	params.anisotropy = 16.0f;

	m_heightMap = Texture2D( 513, 513, &heightMap[ 0 ], InternalFormat::AlphaFloat, PixelFormat::Alpha, PixelType::Float );
	m_heightMap.setParameters( params );

	m_normalMap = Loader::loadTexture2D( "Terrain/NormalMap.png", params );
	m_flowMask = Loader::loadTexture2D( "Terrain/FlowMask.png", params );
	m_cliffMask = Loader::loadTexture2D( "Terrain/CliffMask.png", params );
	m_densityMap = Loader::loadTexture2D( "Terrain/DensityMap.png", params );

	params.s = params.t = TextureWrap::Repeat;

	//m_grassDiffuse = Loader::loadLinearTexture2D( "Terrain/Diffuse/Grass.jpg", params );
	m_cliffDiffuse = Loader::loadTexture2D( "Terrain/Diffuse/Cliff.jpg", params );
}

void Terrain::initUniforms( const Shader& shader )
{
	shader.bind();
	//shader.setUniform( "normalMap", 0 );
	shader.setUniform( "flowMask", 1 );
	shader.setUniform( "cliffMask", 2 );
	//shader.setUniform( "grassDiffuse", 3 );
	shader.setUniform( "cliffDiffuse", 4 );
	//shader.setUniform( "shadowMap", 5 );
}

void Terrain::setUniforms( const Shader& shader, const Scene& scene, const Settings& settings ) const
{
	shader.setUniform( "viewProj", scene.getCamera().getMatrix() );
	shader.setUniform( "world", glm::mat4() );
	//shader.setUniform( "lightPos", settings.lightPos );
	//shader.setUniform( "lightIntensity", settings.lightIntensity );

	shader.setUniform( "squareAltitude", settings.squareAltitude );
	shader.setUniform( "maxHeight", settings.maxHeight );
	shader.setUniform( "snowHeight", settings.snowHeight );
	shader.setUniform( "flowStrength", settings.flowStrength );
	shader.setUniform( "flowHeight", settings.flowHeight );
}

void Terrain::bindTextures() const
{
	m_normalMap.bind( 0 );
	m_flowMask.bind( 1 );
	m_cliffMask.bind( 2 );

	//m_grassDiffuse.bind( 3 );
	m_cliffDiffuse.bind( 4 );
}

void Terrain::render( RenderTarget& target ) const
{
	m_mesh.render( target );
}

const Texture2D& Terrain::getHeightMap() const
{
	return m_heightMap;
}

const Texture2D& Terrain::getDensityMap() const
{
	return m_densityMap;
}