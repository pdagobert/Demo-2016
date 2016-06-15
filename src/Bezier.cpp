#include "Bezier.hpp"
#include "Application.hpp"
#include <fstream>
#include <stdexcept>

glm::vec3 bezierCurve( glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float f )
{
	float t = 1.0f - f;
	float b0 = t * t * t;
	float b1 = 3.0f * f * t * t;
	float b2 = 3.0f * f * f * t;
	float b3 = f * f * f;

	return p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;
}

Bezier::Bezier()
{

}

void Bezier::save( const std::string& ) const
{
	/*std::ofstream file( fileName );

	for( const Track& track : m_tracks )
		file << track.p0.x << " " << track.p0.y << " " << track.p0.z << " " <<
		track.p1.x << " " << track.p1.y << " " << track.p1.z << " " <<
		track.c0.x << " " << track.c0.y << " " << track.c0.z << " " <<
		track.c1.x << " " << track.c1.y << " " << track.c1.z << " " <<
		track.time << std::endl;

	file.close();*/
}
#include <iostream>
void Bezier::load( const std::string& fileName )
{
	std::ifstream file( fileName );
	if( file.fail() )
		throw std::runtime_error( fileName + " not found" );

	Track track;

	while( file >> track.p0.x >> track.p0.y >> track.p0.z >> track.p1.x >> track.p1.y >> track.p1.z >> track.c0.x >> track.c0.y >> track.c0.z >> track.c1.x >> track.c1.y >> track.c1.z >>
		track.dir.x >> track.dir.y >> track.dir.z >> track.dir2.x >> track.dir2.y >> track.dir2.z >> track.light.x >> track.light.y >> track.light.z >> track.time )
		m_tracks.push_back( track );

	std::cout << "size : " << m_tracks.size() << std::endl;
}

void Bezier::addTrack( const Track& track )
{
	m_tracks.push_back( track );
}

TrackIt Bezier::getFirstTrack() const
{
	return m_tracks.begin();
}

TrackIt Bezier::getNextTrack( TrackIt iterator ) const
{
	return ++iterator;
}

bool Bezier::isLast( TrackIt iterator ) const
{
	return ( getNextTrack( iterator ) == m_tracks.end() );
}

bool Bezier::isEmpty() const
{
	return m_tracks.empty();
}

BezierAnimator::BezierAnimator( Camera& camera, const Bezier& bezier ) :
	m_timer( 0.0f ),
	m_camera( camera ),
	m_running( true ),
	m_currentTrack( bezier.getFirstTrack() ),
	m_bezier( bezier )
{
	m_camera.setPosition( m_currentTrack->p0 );
	//m_camera.setSpeed( m_currentTrack->speed );
}

void BezierAnimator::update( float deltaTime, Settings& setting )
{
	float t = m_timer;
	bool trackOver = false;

	if( t >= 1.0f )
	{
		t = 1.0f;
		trackOver = true;
	}

	glm::vec3 currentPos = bezierCurve( m_currentTrack->p0, m_currentTrack->c0, m_currentTrack->c1, m_currentTrack->p1, t );
	m_camera.setPosition( currentPos );
	m_camera.setFront( glm::mix( m_currentTrack->dir, m_currentTrack->dir2, t ) );
	setting.lightPos = m_currentTrack->light;

	if( trackOver )
	{
		if( !m_bezier.isLast( m_currentTrack ) )
		{
			m_currentTrack = m_bezier.getNextTrack( m_currentTrack );
			m_timer = 0.0f;
		}
		else
			m_running = false;
	}

	m_timer += deltaTime / m_currentTrack->time;
}

bool BezierAnimator::isOver() const
{
	return !m_running;
}

/*Camera& BezierAnimator::getCameraRef()
{
	return m_camera;
}

const Camera& BezierAnimator::getCamera() const
{
	return m_camera;
}*/