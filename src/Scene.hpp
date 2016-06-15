#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include <vector>
#include "OpenGL/Camera.hpp"
#include "Terrain.hpp"
#include "Water.hpp"
#include "Objects.hpp"
#include "Grass.hpp"
#include "Skybox.hpp"
#include "Bezier.hpp"
#include <memory>

enum class ObjectType
{
	Terrain = 0x1,
	Water = 0x2,
	Objects = 0x4,
	Grass = 0x8,
	Skybox = 0x10,
	Everything = Terrain | Water | Objects | Grass | Skybox
};

inline ObjectType operator|( ObjectType a, ObjectType b )
{
	return static_cast< ObjectType >( static_cast< int >( a ) | static_cast< int >( b ) );
}

inline ObjectType operator&( ObjectType a, ObjectType b )
{
	return static_cast< ObjectType >( static_cast< int >( a ) & static_cast< int >( b ) );
}

inline ObjectType operator~( ObjectType a )
{
	return static_cast< ObjectType >( ~static_cast< int >( a ) );
}

inline bool isType( ObjectType a, ObjectType b )
{
	return static_cast< int >( a & b );
}

struct settings;

class Scene
{
public:
	Scene( int windowWidth, int windowHeight );

	void update( float deltaTime, Settings& settings );

	void free();

	Camera& getCameraRef();
	const Camera& getCamera() const;

	const Terrain& getTerrain() const;
	const Water& getWater() const;
	const std::vector< Objects >& getObjects() const;
	const std::vector< Grass >& getGrassPatches() const;
	const Skybox& getSkybox() const;

	const Texture2D& getHeightMap() const;

private:
	Camera m_camera;

	Terrain m_terrain;
	Water m_water;

	std::vector< Objects > m_objects;
	std::vector< Grass > m_grassPatches;

	Skybox m_skybox;

	Bezier m_curve;
	std::unique_ptr< BezierAnimator > m_animator;
};
#endif