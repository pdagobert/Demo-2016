#include "Scene.hpp"
#include "InstanceGenerator.hpp"
#include "Application.hpp"
#include <cstdlib>

Scene::Scene( int windowWidth, int windowHeight ) :
	m_camera( windowWidth, windowHeight, glm::vec3( 0.0f ), 10.0f ),
	m_skybox( "Textures/Skybox/", ".png" )
{
	srand( 464 );

	InstanceGenerator generator;

	std::vector< float > pixels = m_terrain.getDensityMap().getFloatPixels( PixelFormat::Alpha );


	float area = 513.0f;

	auto instances = generator.addObjects( 1500, 10000, area, 1.0f, 1.0f, pixels, glm::vec3( 1.5f ) );

	m_objects.push_back( Objects( "Meshes/Tree_01.obj", 0, "Meshes/Textures/Tree_01_Bark_Diffuse.jpg", "Meshes/Textures/Default_Mask.jpg", instances ) );

	m_objects.push_back( Objects( "Meshes/Tree_01.obj", 1, "Meshes/Textures/Tree_01_Leaves_Diffuse.png", "Meshes/Textures/Tree_01_Leaves_Mask.png", instances ) );

	instances = generator.addObjects( 1500, 10000, area, 1.0f, 1.0f, pixels, glm::vec3( 0.01f ) );

	m_objects.push_back( Objects( "Meshes/Tree_02.obj", 0, "Meshes/Textures/Tree_02_Bark_Diffuse.png", "Meshes/Textures/Default_Mask.jpg", instances ) );

	m_objects.push_back( Objects( "Meshes/Tree_02.obj", 1, "Meshes/Textures/Tree_02_Leaves_Diffuse.png", "Meshes/Textures/Tree_02_Leaves_Mask.png", instances ) );

	m_objects.push_back( Objects( "Meshes/Fern.obj", "Meshes/Textures/Fern_Diffuse.png", "Meshes/Textures/Fern_Mask.png", generator.addObjects( 1000, 10000, area, 1.0f, 1.0f, pixels, glm::vec3( 2.5f ) ) ) );

	m_objects.push_back( Objects( "Meshes/PalmTree.obj", "Meshes/Textures/PalmTree_Diffuse.jpg", "Meshes/Textures/PalmTree_Mask.jpg",
		generator.addObjects( 1500, 10000, area, 1.0f, 1.0f, pixels, glm::vec3( 0.05f ) ) ) );

	m_objects.push_back( Objects( "Meshes/Bush.obj", "Meshes/Textures/Bush_Diffuse.jpg", "Meshes/Textures/Bush_Mask.jpg",
		generator.addObjects( 1500, 10000, area, 1.0f, 1.0f, pixels, glm::vec3( 0.1f ) ) ) );

	//m_objects.push_back( Objects( "Meshes/Daisy.obj", "Meshes/Textures/Daisy_Diffuse.png", "Meshes/Textures/Default_Mask.jpg",
	//	generator.addObjects( 1500, 10000, area, 1.0f, 1.0f, pixels, glm::vec3( 0.1f ) ) ) );

	m_objects.push_back( Objects( "Meshes/HighTree.obj", "Meshes/Textures/HighTree_Diffuse.jpg", "Meshes/Textures/HighTree_Mask.jpg",
		generator.addObjects( 300, 10000, area, 0.1f, 0.1f, pixels, glm::vec3( 0.1f ) ) ) );

	m_grassPatches.push_back( Grass( generator.generateGrassPatch( pixels ) ) );

	m_curve.load( "Files/tracks.txt" );
	m_animator = std::unique_ptr< BezierAnimator >( new BezierAnimator( m_camera, m_curve ) );

	//m_camera.setFront( glm::vec3( -0.537934, 0.156434, 0.828345 ) );
	//m_camera.setFront( glm::vec3( 0.0f, 1.0f, 0.0f ) );//-0.537934, 0.156434, -0.828345 ) );
}

void Scene::update( float deltaTime, Settings& settings )
{
	if( m_animator )
	{
		m_animator->update( deltaTime, settings );

		if( m_animator->isOver() )
		{
			m_animator.reset();
			m_camera.unlock();
		}
	}
}

void Scene::free()
{
	m_animator.reset();
	m_camera.unlock();
}

Camera& Scene::getCameraRef()
{
	return m_camera;
}

const Camera& Scene::getCamera() const
{
	return m_camera;
}

const Terrain& Scene::getTerrain() const
{
	return m_terrain;
}

const Water& Scene::getWater() const
{
	return m_water;
}

const std::vector< Objects >& Scene::getObjects() const
{
	return m_objects;
}

const std::vector< Grass >& Scene::getGrassPatches() const
{
	return m_grassPatches;
}

const Skybox& Scene::getSkybox() const
{
	return m_skybox;
}

const Texture2D& Scene::getHeightMap() const
{
	return m_terrain.getHeightMap();
}