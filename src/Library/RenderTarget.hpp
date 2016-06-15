#ifndef __RENDER_TARGET_HPP__
#define __RENDER_TARGET_HPP__

#include <map>
#include <string>
#include <memory>
#include "OpenGL/Viewport.hpp"
#include "OpenGL/FrameBuffer.hpp"
#include "OpenGL/Texture.hpp"
#include "OpenGL/RenderBuffer.hpp"

class RenderCommand;

class RenderTarget
{
public:
	RenderTarget( const Viewport& viewport );

	RenderTarget( RenderTarget&& target );
	RenderTarget& operator=( RenderTarget&& target );

	RenderTarget( const RenderTarget& ) = delete;
	RenderTarget& operator=( const RenderTarget& ) = delete;

	static RenderTarget defaultRenderTarget( const Viewport& viewport );

	bool isValid() const;

	void prepare();
	void draw( const RenderCommand& command );

	void attach( Attachment attachment, Texture2D&& texture );
	void attach( Attachment attachment, RenderBuffer&& renderBuffer );
	//void attach( Attachment attachment, const std::string& name, CubeMap&& cubeMap, int face );

	const Texture2D& getTexture( Attachment attachment ) const;
	Texture2D& getTextureRef( Attachment attachment );

private:
	RenderTarget();

private:
	Viewport m_viewport;
	std::unique_ptr< FrameBuffer > m_frameBuffer;

	std::map< Attachment, Texture2D > m_textures;
	std::map< Attachment, RenderBuffer > m_renderBuffers;
};
#endif