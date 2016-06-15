#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

#include "OpenGL/Window.hpp"
#include "Scene.hpp"
#include "Pipeline.hpp"

enum class DebugTexture
{
	Default,
	Occlusion,
	RadialBlur,
	ShadowMap,
	ReflectionMap,
	RefractionMap,
	WaterDepth,
	FullScene,
	Brightness,
	Blur,
};

struct Settings
{
	bool wireframe = false;
	bool bloomEnabled = true;
	bool shadows = true;

	DebugTexture debugTexture;
	float cameraSpeed = 200.0f;

	glm::vec3 lightPos = glm::normalize( glm::vec3( -0.44f, 0.28f, 0.85f ) );
	float lightIntensity = 1.0f;

	float brightness = 0.0f;
	bool toneMappingEnabled = true;
	float exposure = 1.0f;

	bool squareAltitude = true;
	float maxHeight = 70.0f;
	float snowHeight = 40.0f;
	float flowStrength = 20.0f;
	float flowHeight = -15.0f;

	int textureIndex = 1;
	float waterHeight = 4.2f;
	float textureRepeat = 10.0f;
	float speed = 0.01f;
	float distortionStrength = 0.001f;
	float softEdgesSize = 1.0f;
	bool fixedFresnelNormal = false;
	float fresnelStrength = 1.0f;

	float shadowMapSize = 260.0f;
	float shadowMapFar = 700.0f;
	float shadowMapDist = 500.0f;
	float bias = 0.0f;

	int grassTexture = 2;

	float decay = 0.97875f;
	float intensity = 0.125;
	int samples = 200;
	float godraysExposure = 0.6f;
	float sunScale = 50.0f;

	bool fullRays = true;
};

class Application : public Window
{
public:
	Application();

private:
	virtual void render( float );

	virtual void processEvents( float deltaTime );
	virtual void mouseMoved( float x, float y );
	virtual void keyPressed( int key );

	void initMusic();
	void setupUI();

private:
	Settings m_settings;
	
	Scene m_scene;
	Pipeline m_pipeline;

	bool m_showDebug = false;

	float m_timer = 0.0f;
	int m_volume;
};
#endif