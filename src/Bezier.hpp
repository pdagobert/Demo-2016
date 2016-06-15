#ifndef __BEZIER_HPP__
#define __BEZIER_HPP__

#include "OpenGL/Camera.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Settings;

glm::vec3 bezierCurve( glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float f );

class Track
{
public:
	float time;

	glm::vec3 p0, p1;
	glm::vec3 c0, c1;

	glm::vec3 dir, dir2;

	glm::vec3 light;
};

typedef std::vector< Track >::const_iterator TrackIt;

class Bezier
{
public:
	Bezier();

	void save( const std::string& fileName ) const;
	void load( const std::string& fileName );

	void addTrack( const Track& track );

	TrackIt getFirstTrack() const;
	TrackIt getNextTrack( TrackIt iterator ) const;

	bool isLast( TrackIt iterator ) const;
	bool isEmpty() const;

private:	
	std::vector< Track > m_tracks;
};

class BezierAnimator
{
public:
	BezierAnimator( Camera& camera, const Bezier& bezier );

	void update( float deltaTime, Settings& setting );

	bool isOver() const;

	//Camera& getCameraRef();
	//const Camera& getCamera() const;

private:
	float m_timer;
	Camera& m_camera;
	bool m_running;
	TrackIt m_currentTrack;

	const Bezier& m_bezier;
};
#endif