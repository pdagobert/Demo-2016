#ifndef __WATER_HPP__
#define __WATER_HPP__

#include "OpenGL/Shader.hpp"
#include "Mesh.hpp"
#include "OpenGL/Texture.hpp"

class RenderTarget;
class Shader;
class Scene;
struct Settings;

class Water
{
public:
	Water();

	void setUniforms( const Shader& shader, const Scene& scene, const Settings& settings ) const;
	void bindTextures( const Settings& settings, const Texture2D& reflection, const Texture2D& refraction, const Texture2D& depth ) const;
	void render( RenderTarget& target ) const;

	static void initUniforms( const Shader& shader );

private:
	Mesh m_waterPlane;

	std::vector< Texture2D > m_dudvMaps, m_normalMaps;
};
#endif