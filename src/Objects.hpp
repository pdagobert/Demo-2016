#ifndef __OBJECTS_HPP__
#define __OBJECTS_HPP__

#include "Mesh.hpp"
#include "OpenGL/Texture.hpp"
#include <string>

class RenderTarget;
class Shader;
class Scene;
struct Settings;

class Objects
{
public:
	Objects( const std::string& mesh, const std::string& diffuse, const std::string& mask, const std::vector< Instance >& instances );
	Objects( const std::string& mesh, int meshIndex, const std::string& diffuse, const std::string& mask, const std::vector< Instance >& instances );

	void setUniforms( const Shader& shader, const Scene& scene, const Settings& settings ) const;
	void bindTextures( const Scene& scene ) const;
	void render( RenderTarget& target ) const;

	static void initUniforms( const Shader& shader );

private:
	InstancedMesh m_mesh;
	Texture2D m_diffuse, m_mask;
};
#endif