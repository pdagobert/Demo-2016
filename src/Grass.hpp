#ifndef __GRASS_HPP__
#define __GRASS_HPP__

#include "Mesh.hpp"
#include "OpenGL/Texture.hpp"
#include <vector>

class RenderTarget;
class Shader;
class Scene;
struct Settings;

class Grass
{
public:
	Grass( const std::vector< Instance >& instances );

	void setUniforms( const Shader& shader, const Scene& scene, const Settings& settings ) const;
	void bindTextures( const Scene& scene, const Settings& settings ) const;
	void render( RenderTarget& target ) const;

	static void initUniforms( const Shader& shader );

private:
	InstancedMesh m_mesh;
	std::vector< Texture2D > m_diffuseMaps;
};
#endif