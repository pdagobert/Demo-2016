#ifndef __TERRAIN_HPP__
#define __TERRAIN_HPP__

#include "Mesh.hpp"
#include "OpenGL/Texture.hpp"

class RenderTarget;
class Shader;
class Scene;
struct Settings;

class Terrain
{
public:
	Terrain();

	void setUniforms( const Shader& shader, const Scene& scene, const Settings& settings ) const;
	void bindTextures() const;
	void render( RenderTarget& target ) const;

	static void initUniforms( const Shader& shader );

	const Texture2D& getHeightMap() const;
	const Texture2D& getDensityMap() const;

private:
	Mesh m_mesh;

	Texture2D m_normalMap, m_flowMask, m_cliffMask;
	Texture2D m_grassDiffuse, m_cliffDiffuse;
	Texture2D m_heightMap, m_densityMap;
};
#endif