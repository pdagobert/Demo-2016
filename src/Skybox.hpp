#ifndef __SKYBOX_HPP__
#define __SKYBOX_HPP__

#include "Mesh.hpp"
#include "OpenGL/Texture.hpp"
#include <string>

class RenderTarget;
class Shader;
class Scene;
struct Settings;

class Camera;

class Skybox
{
public:
	Skybox( const std::string& path, const std::string& ext );

	void setUniforms( const Shader& shader, const Scene& scene ) const;
	void bindTextures() const;
	void render( RenderTarget& target ) const;

	static void initUniforms( const Shader& shader );

private:
	Mesh m_cube;
	CubeMap m_cubeMap;
};
#endif