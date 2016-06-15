#include "Application.hpp"
#include <ATB/AntTweakBar.h>
#include <SDL2/SDL_mixer.h>

const int windowWidth = 1280;
const int windowHeight = 1024;

Application::Application() :
	Window( windowWidth, windowHeight, "OpenGL" ),
	m_settings(),
	m_scene( windowWidth, windowHeight ),
	m_pipeline( windowWidth, windowHeight )
{
	glfwSwapInterval( 0 );

	setupUI();
	initMusic();
}

void Application::initMusic()
{
	int flags = MIX_INIT_MP3;
	Mix_Init( flags );

	if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 1024 ) != 0 )
		throw std::runtime_error( "SDL Mixer init failed" );

	Mix_Music* music = Mix_LoadMUS( "Music/Tony Anderson - Breakthrough.mp3" );
	if( !music )
		throw std::runtime_error( "Music not found" );

	Mix_PlayMusic( music, 0 );

	m_volume = Mix_VolumeMusic( -1 );
}

void Application::setupUI()
{
	TwBar* bar = TwNewBar( "Parameters" );
	TwAddVarRW( bar, "wireframe", TW_TYPE_BOOLCPP, &m_settings.wireframe, "group=global" );
	TwAddVarRW( bar, "bloomEnabled", TW_TYPE_BOOLCPP, &m_settings.bloomEnabled, "group=global" );
	TwAddVarRW( bar, "hasShadows", TW_TYPE_BOOLCPP, &m_settings.shadows, "group=global" );

	TwType type = TwDefineEnum( "DebugTexture", nullptr, 0 );
	TwAddVarRW( bar, "debugTexture", type, &m_settings.debugTexture, "group=global enum='0{Default}, 1{Occlusion}, 2{RadialBlur}, 3{ShadowMap}, 4{Reflection}, 5{Refraction}, 6{WaterDepth}, 7{FullScene}, 8{Brighness}, 9{Blur}, 10{Bloom}'" );

	TwAddVarRW( bar, "cameraSpeed", TW_TYPE_FLOAT, &m_settings.cameraSpeed, "group=global" );
	TwAddVarRW( bar, "lightPos", TW_TYPE_DIR3F, &m_settings.lightPos, "group=light" );
	TwAddVarRW( bar, "lightIntensity", TW_TYPE_FLOAT, &m_settings.lightIntensity, "group=light" );

	TwAddVarRW( bar, "brightness", TW_TYPE_FLOAT, &m_settings.brightness, "group=bloom" );
	TwAddVarRW( bar, "toneMappingEnabled", TW_TYPE_BOOLCPP, &m_settings.toneMappingEnabled, "group=bloom" );
	TwAddVarRW( bar, "exposure", TW_TYPE_FLOAT, &m_settings.exposure, "group=bloom" );

	TwAddVarRW( bar, "squareAltitude", TW_TYPE_BOOLCPP, &m_settings.squareAltitude, "group=terrain" );
	TwAddVarRW( bar, "maxHeight", TW_TYPE_FLOAT, &m_settings.maxHeight, "group=terrain" );
	TwAddVarRW( bar, "snowHeight", TW_TYPE_FLOAT, &m_settings.snowHeight, "group=terrain" );
	TwAddVarRW( bar, "flowStrength", TW_TYPE_FLOAT, &m_settings.flowStrength, "group=terrain" );
	TwAddVarRW( bar, "flowHeight", TW_TYPE_FLOAT, &m_settings.flowHeight, "group=terrain" );

	TwAddVarRW( bar, "textureIndex", TW_TYPE_INT32, &m_settings.textureIndex, "group=water min=0 max=4" );
	TwAddVarRW( bar, "waterHeight", TW_TYPE_FLOAT, &m_settings.waterHeight, "group=water" );
	TwAddVarRW( bar, "textureRepeat", TW_TYPE_FLOAT, &m_settings.textureRepeat, "group=water" );
	TwAddVarRW( bar, "speed", TW_TYPE_FLOAT, &m_settings.speed, "group=water min=0.0" );
	TwAddVarRW( bar, "distortionStrength", TW_TYPE_FLOAT, &m_settings.distortionStrength, "group=water" );
	TwAddVarRW( bar, "softEdgesSize", TW_TYPE_FLOAT, &m_settings.softEdgesSize, "group=water" );
	TwAddVarRW( bar, "fixedFresnelNormal", TW_TYPE_BOOLCPP, &m_settings.fixedFresnelNormal, "group=water" );
	TwAddVarRW( bar, "fresnelStrength", TW_TYPE_FLOAT, &m_settings.fresnelStrength, "group=water" );

	TwAddVarRW( bar, "shadowMapSize", TW_TYPE_FLOAT, &m_settings.shadowMapSize, "group=shadows" );
	TwAddVarRW( bar, "shadowMapFar", TW_TYPE_FLOAT, &m_settings.shadowMapFar, "group=shadows" );
	TwAddVarRW( bar, "shadowMapDist", TW_TYPE_FLOAT, &m_settings.shadowMapDist, "group=shadows" );
	TwAddVarRW( bar, "bias", TW_TYPE_FLOAT, &m_settings.bias, "group=shadows" );

	TwAddVarRW( bar, "grassTexture", TW_TYPE_INT32, &m_settings.grassTexture, "group=grass min=0 max=2" );

	TwAddVarRW( bar, "decay", TW_TYPE_FLOAT, &m_settings.decay, "group=godrays" );
	TwAddVarRW( bar, "intensity", TW_TYPE_FLOAT, &m_settings.intensity, "group=godrays" );
	TwAddVarRW( bar, "samples", TW_TYPE_INT32, &m_settings.samples, "group=godrays" );
	TwAddVarRW( bar, "godraysExposure", TW_TYPE_FLOAT, &m_settings.godraysExposure, "group=godrays" );
	TwAddVarRW( bar, "sunScale", TW_TYPE_FLOAT, &m_settings.sunScale, "group=godrays" );

	TwAddVarRW( bar, "fullRays", TW_TYPE_BOOLCPP, &m_settings.fullRays, "group=godrays" );
}

void Application::render( float deltaTime )
{
	m_scene.update( deltaTime, m_settings );

	m_scene.getCameraRef().setSpeed( m_settings.cameraSpeed );

	float intensity = 1.0f;

	if( m_timer < 8.0f )
	{
		intensity = m_timer / 8.0f;
	}
	else if( m_timer > 207.0f )
	{
		intensity = ( m_timer - 207.0f ) / ( 217.0f - 207.0f );

		Mix_VolumeMusic( m_volume - intensity * m_volume );

		intensity = 1.0 - intensity;
	}
	else if( m_timer > 217.0f )
	{
		close();
		exit( 0 );
	}

	m_pipeline.render( m_scene, m_settings, intensity );


	if( m_showDebug )
		TwDraw();

	m_timer += deltaTime;
}

void Application::processEvents( float deltaTime )
{
	Camera& camera = m_scene.getCameraRef();

	if( isKeyPressed( GLFW_KEY_A ) )
		camera.moveLeft( deltaTime );

	if( isKeyPressed( GLFW_KEY_D ) )
		camera.moveRight( deltaTime );

	if( isKeyPressed( GLFW_KEY_W ) )
		camera.moveForward( deltaTime );

	if( isKeyPressed( GLFW_KEY_S ) )
		camera.moveBackward( deltaTime );

	if( isKeyPressed( GLFW_KEY_SPACE ) )
	{
		glm::vec3 pos = m_scene.getCamera().getPosition();
		glm::vec3 dir = m_scene.getCamera().getFront();

		std::cout << "pos : " << pos.x << " " << pos.y << " " << pos.z << std::endl << "dir : " << dir.x << " " << dir.y << dir.z << std::endl;
	}
}

void Application::mouseMoved( float x, float y )
{
	static float xOld = x, yOld = y;

	if( isButtonPressed( GLFW_MOUSE_BUTTON_RIGHT ) )
		m_scene.getCameraRef().orientate( x - xOld, yOld - y );

	xOld = x;
	yOld = y;
}

void Application::keyPressed( int key )
{
	if( key == GLFW_KEY_O )
		m_showDebug = !m_showDebug;
	else if( key == GLFW_KEY_P )
		m_scene.free();
	else if( key == GLFW_KEY_I )
		close();
}