#include "Pipeline.hpp"
#include "VertexLayout.hpp"
#include "Geometry.hpp"
#include "OpenGL/Camera.hpp"
#include "Scene.hpp"
#include "Application.hpp"
#include "Loader.hpp"
#include <SOIL/SOIL.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cstdlib>
Pipeline::Pipeline( int windowWidth, int windowHeight )
{
	std::cout << (int)ObjectType::Everything << std::endl;

	loadShaders();
	createRenderTargets( windowWidth, windowHeight );

	VertexLayout layout;
	layout.addAttribute( VertexAttribute::Position, 3 );
	layout.addAttribute( VertexAttribute::TexCoords, 2 );

	m_quad = Mesh( createUnitQuad( layout ), Primitive::Triangles );
	m_sphere = Mesh( createUVSphere( 0.5f, 20, layout ), Primitive::Triangles );
}

void Pipeline::render( Scene& scene, const Settings& settings, float globalIntensity )
{
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glEnable( GL_TEXTURE_CUBE_MAP_SEAMLESS );

	glEnable( GL_CLIP_DISTANCE0 );

	glm::vec3 lightPos = glm::normalize( settings.lightPos ) * settings.shadowMapDist;
	lightPos += glm::vec3( 256.0f, 0.0f, -256.0f );

	glm::mat4 lightViewProj = //glm::perspective( glm::radians( 45.0f ), 1.0f, 0.1f, settings.shadowMapFar ) *
		glm::ortho( -settings.shadowMapSize, settings.shadowMapSize, -settings.shadowMapSize, settings.shadowMapSize, 0.1f, settings.shadowMapFar ) *

		glm::lookAt( lightPos, glm::vec3( 256.0f, 0.0f, -256.0f ) + glm::vec3( 1.0, 0.0, 1.0 ), glm::vec3( 0.0f, 1.0f, 0.0f ) );

	{
		RenderTarget& target = m_targets[ Type::Occlusion ];
		target.prepare();

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		if( !settings.fullRays )
		{
			glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );

			renderObjects( ObjectType::Everything & ( ~ObjectType::Skybox | ~ObjectType::Grass ), target, scene, settings, glm::vec4(), glm::mat4() );

			glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
		}
		else
		{
			const Shader& shader = m_shaders[ "BlackObjects" ];
			shader.bind();
			shader.setUniform( "viewProj", scene.getCamera().getMatrix() );

			for( const Objects& objects : scene.getObjects() )
			{
				objects.bindTextures( scene );
				objects.render( target );
			}

			renderObjects( ObjectType::Skybox, target, scene, settings, glm::vec4(), glm::mat4() );

			const Shader& shader2 = m_shaders[ "BlackTerrain" ];
			shader2.bind();
			shader2.setUniform( "viewProj", scene.getCamera().getMatrix() );

			scene.getTerrain().render( target );
		}

		const Shader& shader3 = m_shaders[ "Color" ];
		shader3.bind();
		shader3.setUniform( "viewProj", scene.getCamera().getMatrix() );
		shader3.setUniform( "world", glm::translate( glm::mat4(), lightPos ) * glm::scale( glm::mat4(), glm::vec3( settings.sunScale ) ) );

		m_sphere.render( target );
	}

	{
		RenderTarget& target = m_targets[ Type::RadialBlur ];
		target.prepare();

		glClear( GL_COLOR_BUFFER_BIT );

		const Shader& shader = m_shaders[ "RadialBlur" ];
		shader.bind();

		glm::vec4 ndcLightPos = scene.getCamera().getMatrix() * glm::vec4( lightPos, 1.0f );
		glm::vec3 texCoords = glm::vec3( ndcLightPos ) / ndcLightPos.w;
		texCoords = texCoords * 0.5f + glm::vec3( 0.5f );

		shader.setUniform( "ndcLightPos", texCoords );

		shader.setUniform( "decay", settings.decay );
		shader.setUniform( "intensity", settings.intensity );
		shader.setUniform( "samples", settings.samples );
		shader.setUniform( "exposure", settings.godraysExposure );

		m_targets[ Type::Occlusion ].getTexture( Attachment::Color0 ).bind( 0 );

		m_quad.render( target );
	}

	{
		RenderTarget& target = m_targets[ Type::Shadow ];
		target.prepare();

		glClear( GL_DEPTH_BUFFER_BIT );

		const Shader& shader = m_shaders[ "Shadow" ];
		shader.bind();

		shader.setUniform( "lightViewProj", lightViewProj );

		scene.getHeightMap().bind( 0 );

		//glEnable( GL_CULL_FACE );

		if( settings.shadows )
		{
			for( const Objects& objects : scene.getObjects() )
			{
				objects.bindTextures( scene );
				objects.render( target );
			}
		}

		//glDisable( GL_CULL_FACE );
	}

	glm::vec3 oldPosition = modifyCamera( scene.getCameraRef(), settings.waterHeight );

	{
		RenderTarget& target = m_targets[ Type::Reflection ];
		target.prepare();

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		renderObjects( ObjectType::Everything & ~ObjectType::Water, target, scene, settings, glm::vec4( 0.0f, 1.0f, 0.0f, -settings.waterHeight ), lightViewProj );

		//target.getTextureRef( Attachment::Color0 ).generateMipMaps();
	}

	restoreCamera( scene.getCameraRef(), oldPosition );

	{
		RenderTarget& target = m_targets[ Type::Refraction ];
		target.prepare();

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		renderObjects( ObjectType::Everything & ~ObjectType::Water, target, scene, settings, glm::vec4( 0.0f, -1.0f, 0.0f, settings.waterHeight + 0.01 ), lightViewProj );
	}

	glDisable( GL_CLIP_DISTANCE0 );

	{
		RenderTarget& target = m_targets[ Type::FullScene ];
		target.prepare();

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		renderObjects( ObjectType::Everything & ~ObjectType::Water, target, scene, settings, glm::vec4( 0.0f, 1.0, 0.0, -settings.waterHeight ), lightViewProj );

		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		renderObjects( ObjectType::Water, target, scene, settings, glm::vec4( 0.0f, 1.0, 0.0, -settings.waterHeight ), lightViewProj );

		/*const Shader& shader = m_shaders[ "Color" ];
		shader.bind();
		shader.setUniform( "viewProj", scene.getCamera().getMatrix() );
		shader.setUniform( "world", glm::translate( glm::mat4(), lightPos ) * glm::scale( glm::mat4(), glm::vec3( settings.sunScale ) ) );

		m_sphere.render( target );*/

		glDisable( GL_BLEND );
	}

	glDisable( GL_DEPTH_TEST );

	{
		RenderTarget& target = m_targets[ Type::PostProcess ];
		target.prepare();

		glClear( GL_COLOR_BUFFER_BIT );

		const Shader& shader = m_shaders[ "PostProcess" ];
		shader.bind();
		shader.setUniform( "brightness", settings.brightness );

		m_targets[ Type::FullScene ].getTexture( Attachment::Color0 ).bind( 0 );

		m_quad.render( target );
	}

	{
		Type currentTarget = Type::Blur2;

		int blurIterations = 4;

		const Shader& shader = m_shaders[ "Blur" ];
		shader.bind();

		for( int i = 0; i < blurIterations; i++ )
		{
			RenderTarget& target = m_targets[ currentTarget ];
			target.prepare();

			glClear( GL_COLOR_BUFFER_BIT );

			shader.setUniform( "isHorizontal", currentTarget == Type::Blur1 ? 1 : 0 );

			if( currentTarget == Type::Blur1 )
				currentTarget = Type::Blur2;
			else
				currentTarget = Type::Blur1;

			if( i == 0 )
				m_targets[ Type::PostProcess ].getTexture( Attachment::Color1 ).bind( 0 );
			else
				m_targets[ currentTarget ].getTexture( Attachment::Color0 ).bind( 0 );

			m_quad.render( target );
		}
	}

	{
		RenderTarget& target = m_targets[ Type::ToneMapping ];
		target.prepare();

		const Shader& shader = m_shaders[ "ToneMapping" ];
		shader.bind();
		shader.setUniform( "toneMappingEnabled", settings.toneMappingEnabled );
		shader.setUniform( "bloomEnabled", settings.bloomEnabled );
		shader.setUniform( "exposure", settings.exposure );
		shader.setUniform( "intensity", globalIntensity );

		m_targets[ Type::PostProcess ].getTexture( Attachment::Color0 ).bind( 0 );
		m_targets[ Type::Blur1 ].getTexture( Attachment::Color0 ).bind( 1 );
		m_targets[ Type::RadialBlur ].getTexture( Attachment::Color0 ).bind( 2 );

		m_quad.render( target );
	}

	{
		RenderTarget& target = m_targets[ Type::Default ];
		target.prepare();

		glClear( GL_COLOR_BUFFER_BIT );

		prepareShader( m_shaders[ "Texture" ], settings.debugTexture, scene, settings );
		m_quad.render( target );
	}
}

void Pipeline::renderObjects( ObjectType type, RenderTarget& target, const Scene& scene, const Settings& settings, const glm::vec4& clip, const glm::mat4& lightMatrix )
{
	if( isType( type, ObjectType::Terrain ) )
	{
		const Shader& shader = m_shaders[ "Terrain" ];
		shader.bind();
		shader.setUniform( "clip", clip );
		//shader.setUniform( "shadowBias", settings.bias );
		//shader.setUniform( "lightViewProj", lightMatrix );

		m_targets[ Type::Shadow ].getTexture( Attachment::Depth ).bind( 5 );

		const Terrain& terrain = scene.getTerrain();
		terrain.setUniforms( shader, scene, settings );
		terrain.bindTextures();
		terrain.render( target );
	}

	if( isType( type, ObjectType::Water ) )
	{
		const Shader& shader = m_shaders[ "Water" ];
		shader.bind();

		const Water& water = scene.getWater();
		water.setUniforms( shader, scene, settings );
		water.bindTextures( settings, m_targets[ Type::Reflection ].getTexture( Attachment::Color0 ), m_targets[ Type::Refraction ].getTexture( Attachment::Color0 ),
			m_targets[ Type::Refraction ].getTexture( Attachment::Depth ) );

		water.render( target );
	}

	if( isType( type, ObjectType::Objects ) )
	{
		const Shader& shader = m_shaders[ "Objects" ];
		shader.bind();
		shader.setUniform( "clip", clip );

		//m_targets[ Type::Shadow ].getTexture( Attachment::Depth ).bind( 3 );

		for( const Objects& objects : scene.getObjects() )
		{
			objects.setUniforms( shader, scene, settings );
			objects.bindTextures( scene );
			objects.render( target );
		}
	}

	if( isType( type, ObjectType::Grass ) )
	{
		const Shader& shader = m_shaders[ "Grass" ];
		shader.bind();
		shader.setUniform( "shadowBias", settings.bias );
		shader.setUniform( "lightViewProj", lightMatrix );

		m_targets[ Type::Shadow ].getTexture( Attachment::Depth ).bind( 2 );

		for( const Grass& patches : scene.getGrassPatches() )
		{
			patches.setUniforms( shader, scene, settings );
			patches.bindTextures( scene, settings );
			patches.render( target );
		}
	}

	if( isType( type, ObjectType::Skybox ) )
	{
		const Shader& shader = m_shaders[ "Skybox" ];
		shader.bind();

		const Skybox& skybox = scene.getSkybox();
		skybox.setUniforms( shader, scene );
		skybox.bindTextures();
		skybox.render( target );
	}

	// etc...
}

glm::vec3 Pipeline::modifyCamera( Camera& camera, float waterHeight ) const
{
	glm::vec3 oldPosition = camera.getPosition();
	glm::vec3 newPosition = oldPosition;
	newPosition.y -= ( newPosition.y - waterHeight ) * 2.0f;

	camera.setPosition( newPosition );
	camera.invertPitch();

	return oldPosition;
}

void Pipeline::restoreCamera( Camera& camera, const glm::vec3& oldPosition ) const
{
	camera.setPosition( oldPosition );
	camera.invertPitch();
}

void Pipeline::prepareShader( const Shader& shader, DebugTexture texture, const Scene& scene, const Settings& settings ) const
{
	shader.bind();
	shader.setUniform( "type", (int)texture );

	Type type;
	Attachment attachment;

	switch( texture )
	{
		case DebugTexture::Default:
		{
			type = Type::ToneMapping;
			attachment = Attachment::Color0;
			break;
		}

		case DebugTexture::Occlusion:
		{
			type = Type::Occlusion;
			attachment = Attachment::Color0;
			break;
		}

		case DebugTexture::RadialBlur:
		{
			type = Type::RadialBlur;
			attachment = Attachment::Color0;
			break;
		}

		case DebugTexture::ShadowMap:
		{
			type = Type::Shadow;
			attachment = Attachment::Depth;

			shader.setUniform( "nearPlane", scene.getCamera().getFar() );
			shader.setUniform( "farPlane", scene.getCamera().getNear() );
			break;
		}

		case DebugTexture::ReflectionMap:
		{
			type = Type::Reflection;
			attachment = Attachment::Color0;
			break;
		}

		case DebugTexture::RefractionMap:
		{
			type = Type::Refraction;
			attachment = Attachment::Color0;
			break;
		}

		case DebugTexture::WaterDepth:
		{
			type = Type::Refraction;
			attachment = Attachment::Depth;
			break;
		}

		case DebugTexture::FullScene:
		{
			type = Type::PostProcess;
			attachment = Attachment::Color0;
			break;
		}

		case DebugTexture::Brightness:
		{
			type = Type::PostProcess;
			attachment = Attachment::Color1;
			break;
		}

		case DebugTexture::Blur:
		{
			type = Type::Blur1;
			attachment = Attachment::Color0;
			break;
		}
	}

	m_targets[ type ].getTexture( attachment ).bind( 0 );
}

void Pipeline::loadShaders()
{
	Shader shader = Loader::loadShader( "Shaders/Terrain.vs", "Shaders/Terrain.fs" );
	Terrain::initUniforms( shader );

	m_shaders[ "Terrain" ] = std::move( shader );

	shader = Loader::loadShader( "Shaders/Water.vs", "Shaders/Water.fs" );
	Water::initUniforms( shader );

	m_shaders[ "Water" ] = std::move( shader );

	shader = Loader::loadShader( "Shaders/Objects.vs", "Shaders/Objects.fs" );
	Objects::initUniforms( shader );

	m_shaders[ "Objects" ] = std::move( shader );

	shader = Loader::loadShader( "Shaders/Skybox.vs", "Shaders/Skybox.fs" );
	Skybox::initUniforms( shader );

	m_shaders[ "Skybox" ] = std::move( shader );

	shader = Loader::loadShader( "Shaders/Texture.vs", "Shaders/Texture.fs" );
	shader.bind();
	shader.setUniform( "frame", 0 );

	m_shaders[ "Texture" ] = std::move( shader );

	shader = Loader::loadShader( "Shaders/Shadow.vs", "Shaders/PassThrough.fs" );
	shader.bind();
	shader.setUniform( "mask", 1 );
	shader.setUniform( "heightMap", 2 );

	m_shaders[ "Shadow" ] = std::move( shader );

	shader = Loader::loadShader( "Shaders/Color.vs", "Shaders/Color.fs" );
	m_shaders[ "Color" ] = std::move( shader );

	shader = Loader::loadShader( "Shaders/PostProcess.vs", "Shaders/PostProcess.fs" );
	shader.bind();
	shader.setUniform( "frame", 0 );

	m_shaders[ "PostProcess" ] = std::move( shader );

	shader = Loader::loadShader( "Shaders/PostProcess.vs", "Shaders/Blur.fs" );
	shader.bind();
	shader.setUniform( "frame", 0 );

	m_shaders[ "Blur" ] = std::move( shader );

	shader = Loader::loadShader( "Shaders/PostProcess.vs", "Shaders/ToneMapping.fs" );
	shader.bind();
	shader.setUniform( "frame", 0 );
	shader.setUniform( "blurredFrame", 1 );
	shader.setUniform( "godrays", 2 );

	m_shaders[ "ToneMapping" ] = std::move( shader );

	shader = Loader::loadShader( "Shaders/PostProcess.vs", "Shaders/RadialBlur.fs" );
	shader.bind();
	shader.setUniform( "frame", 0 );

	m_shaders[ "RadialBlur" ] = std::move( shader );

	shader = Loader::loadShader( "Shaders/Grass.vs", "Shaders/Grass.fs" );
	Grass::initUniforms( shader );

	m_shaders[ "Grass" ] = std::move( shader );

	shader = Loader::loadShader( "Shaders/Objects.vs", "Shaders/BlackObjects.fs" );
	shader.bind();
	shader.setUniform( "mask", 1 );
	shader.setUniform( "heightMap", 2 );

	m_shaders[ "BlackObjects" ] = std::move( shader );

	shader = Loader::loadShader( "Shaders/Terrain.vs", "Shaders/Black.fs" );
	shader.bind();
	shader.setUniform( "world", glm::mat4() );

	m_shaders[ "BlackTerrain" ] = std::move( shader );
}

void Pipeline::createRenderTargets( int width, int height )
{
	Viewport viewport( glm::ivec2( width, height ) );
	Viewport viewport2( glm::ivec2( width, height ) );
	Viewport viewport4( glm::ivec2( width, height ) );
	
	int depthMapSize = 4096;
	Viewport viewport3( glm::ivec2( depthMapSize, depthMapSize ) );

	m_targets.push_back( RenderTarget( viewport ) ); // occlusion
	m_targets.push_back( RenderTarget( viewport ) ); // radial blur
	m_targets.push_back( RenderTarget( viewport3 ) ); // shadowmap
	m_targets.push_back( RenderTarget( viewport2 ) ); // reflection
	m_targets.push_back( RenderTarget( viewport4 ) ); // refraction
	m_targets.push_back( RenderTarget( viewport ) ); // fullscene
	m_targets.push_back( RenderTarget( viewport ) ); // post process
	m_targets.push_back( RenderTarget( viewport ) ); // blur1
	m_targets.push_back( RenderTarget( viewport ) ); // blur2
	m_targets.push_back( RenderTarget( viewport ) ); // tone mapping
	m_targets.push_back( RenderTarget::defaultRenderTarget( viewport ) ); // default
	
	Texture2DParameters params;
	params.min = TextureFilter::Linear;
	params.mag = TextureFilter::Linear;
	params.s = TextureWrap::Clamp;
	params.t = TextureWrap::Repeat;

	m_targets[ Type::Reflection ].attach( Attachment::Color0, Texture2D( viewport2.m_size.x, viewport2.m_size.y, nullptr, InternalFormat::RGBFloat, PixelFormat::RGB, PixelType::Float, params ) );
	m_targets[ Type::Reflection ].attach( Attachment::Depth, RenderBuffer( viewport2.m_size.x, viewport2.m_size.y, Storage::Depth32 ) );

	params.t = TextureWrap::Clamp;

	m_targets[ Type::Occlusion ].attach( Attachment::Color0, Texture2D( viewport.m_size.x, viewport.m_size.y, nullptr, InternalFormat::RGBFloat, PixelFormat::RGB, PixelType::Float, params ) );
	m_targets[ Type::Occlusion ].attach( Attachment::Depth, RenderBuffer( viewport.m_size.x, viewport.m_size.y, Storage::Depth32 ) );

	m_targets[ Type::RadialBlur ].attach( Attachment::Color0, Texture2D( viewport.m_size.x, viewport.m_size.y, nullptr, InternalFormat::RGBFloat, PixelFormat::RGB, PixelType::Float, params ) );

	m_targets[ Type::Refraction ].attach( Attachment::Color0, Texture2D( viewport2.m_size.x, viewport2.m_size.y, nullptr, InternalFormat::RGBFloat, PixelFormat::RGB, PixelType::UByte, params ) );
	m_targets[ Type::Refraction ].attach( Attachment::Depth, Texture2D( viewport2.m_size.x, viewport2.m_size.y, nullptr, InternalFormat::Depth, PixelFormat::Depth, PixelType::Float, params ) );

	m_targets[ Type::Shadow ].attach( Attachment::Depth, Texture2D( depthMapSize, depthMapSize, nullptr, InternalFormat::Depth, PixelFormat::Depth, PixelType::Float, params ) );

	m_targets[ Type::FullScene ].attach( Attachment::Color0, Texture2D( width, height, nullptr, InternalFormat::RGBFloat, PixelFormat::RGB, PixelType::Float, params ) );
	m_targets[ Type::FullScene ].attach( Attachment::Depth, RenderBuffer( width, height, Storage::Depth32 ) );

	m_targets[ Type::PostProcess ].attach( Attachment::Color0, Texture2D( width, height, nullptr, InternalFormat::RGBFloat, PixelFormat::RGB, PixelType::Float, params ) );
	m_targets[ Type::PostProcess ].attach( Attachment::Color1, Texture2D( width, height, nullptr, InternalFormat::RGBFloat, PixelFormat::RGB, PixelType::Float, params ) );
	m_targets[ Type::PostProcess ].attach( Attachment::Depth, RenderBuffer( width, height, Storage::Depth32 ) );

	m_targets[ Type::PostProcess ].prepare();
	GLuint attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers( 2, attachments );

	m_targets[ Type::Blur1 ].attach( Attachment::Color0, Texture2D( viewport.m_size.x, viewport.m_size.y, nullptr, InternalFormat::RGBFloat, PixelFormat::RGB, PixelType::Float, params ) );
	m_targets[ Type::Blur2 ].attach( Attachment::Color0, Texture2D( viewport.m_size.x, viewport.m_size.y, nullptr, InternalFormat::RGBFloat, PixelFormat::RGB, PixelType::Float, params ) );

	m_targets[ Type::ToneMapping ].attach( Attachment::Color0, Texture2D( width, height, nullptr, InternalFormat::RGB, PixelFormat::RGB, PixelType::UByte, params ) );

	for( RenderTarget& target : m_targets )
	{
		if( &target == &m_targets[ Default ] )
			continue;

		if( !target.isValid() )
			throw std::runtime_error( "framebuffer not valid" );
	}

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}