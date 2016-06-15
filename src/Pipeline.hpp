#ifndef __PIPELINE_HPP__
#define __PIPELINE_HPP__

#include "RenderTarget.hpp"
#include "OpenGL/Shader.hpp"
#include "Mesh.hpp"
#include <map>
#include <string>
#include <vector>
#include <glm/glm.hpp>

class Camera;
class Scene;
class Settings;
enum class ObjectType;
enum class DebugTexture;

class Pipeline
{
public:
	Pipeline( int windowWidth, int windowHeight );

	void render( Scene& scene, const Settings& settings, float globalIntensity );

private:
	void loadShaders();
	void createRenderTargets( int width, int height );
	void renderObjects( ObjectType type, RenderTarget& target, const Scene& scene, const Settings& settings, const glm::vec4& clip, const glm::mat4& lightMatrix );

	glm::vec3 modifyCamera( Camera& camera, float waterHeight ) const;
	void restoreCamera( Camera& camera, const glm::vec3& oldPosition ) const;

	void prepareShader( const Shader& shader, DebugTexture texture, const Scene& scene, const Settings& settings ) const;

private:
	std::map< std::string, Shader > m_shaders;
	std::vector< RenderTarget > m_targets;

	Mesh m_quad, m_sphere;

private:
	enum Type
	{
		Occlusion,
		RadialBlur,
		Shadow,
		Reflection,
		Refraction,
		FullScene,
		PostProcess,
		Blur1,
		Blur2,
		ToneMapping,
		Default
	};
};
#endif