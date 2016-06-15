#include "RenderTarget.hpp"
#include "OpenGL/RenderCommand.hpp"

RenderTarget::RenderTarget() :
	m_viewport( glm::ivec2( 0, 0 ) ),
	m_frameBuffer()
{

}

RenderTarget::RenderTarget( const Viewport& viewport ) :
	m_viewport( viewport ),
	m_frameBuffer( new FrameBuffer() )
{

}

RenderTarget::RenderTarget( RenderTarget&& target ) :
	m_viewport( target.m_viewport ),
	m_frameBuffer( std::move( target.m_frameBuffer ) )
{

}

RenderTarget& RenderTarget::operator=( RenderTarget&& target )
{
	m_viewport = target.m_viewport;
	m_frameBuffer = std::move( target.m_frameBuffer );
	return *this;
}

RenderTarget RenderTarget::defaultRenderTarget( const Viewport& viewport )
{
	RenderTarget target;
	target.m_viewport = viewport;

	return target;
}

bool RenderTarget::isValid() const
{
	if( !m_frameBuffer )
		throw std::runtime_error( "default framebuffer is always valid" );
	else
		return m_frameBuffer->isValid();
}

void RenderTarget::prepare()
{
	m_viewport.bind();

	if( m_frameBuffer )
		m_frameBuffer->bind();
	else
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void RenderTarget::draw( const RenderCommand& command )
{
	if( command.instanceCount != 0 )
		glDrawElementsInstanced( static_cast< GLenum >( command.primitive ), command.count, GL_UNSIGNED_INT, reinterpret_cast< const GLvoid* >( command.offset * 4 ), command.instanceCount );
	else
		glDrawElements( static_cast< GLenum >( command.primitive ), command.count, GL_UNSIGNED_INT, reinterpret_cast< const GLvoid* >( command.offset * 4 ) );
}

void RenderTarget::attach( Attachment attachment, Texture2D&& texture )
{
	if( m_textures.find( attachment ) != m_textures.end() )
		throw std::runtime_error( "texture key already exists in render target" );

	if( !m_frameBuffer )
		throw std::runtime_error( "cannot attach texture on default framebuffer" );

	m_textures[ attachment ] = std::move( texture );
	m_frameBuffer->attach( attachment, m_textures[ attachment ] );
}

void RenderTarget::attach( Attachment attachment, RenderBuffer&& renderBuffer )
{
	if( m_renderBuffers.find( attachment ) != m_renderBuffers.end() )
		throw std::runtime_error( "render buffer key already exists in render target" );

	if( !m_frameBuffer )
		throw std::runtime_error( "cannot attach render buffer on default framebuffer" );
	
	m_renderBuffers[ attachment ] = std::move( renderBuffer );
	m_frameBuffer->attach( attachment, m_renderBuffers[ attachment ] );
}

const Texture2D& RenderTarget::getTexture( Attachment attachment ) const
{
	auto it = m_textures.find( attachment );
	if( it == m_textures.end() )
		throw std::runtime_error( "texture key not found in render target" );

	return it->second;
}

Texture2D& RenderTarget::getTextureRef( Attachment attachment )
{
	auto it = m_textures.find( attachment );
	if( it == m_textures.end() )
		throw std::runtime_error( "texture key not found in render target" );

	return it->second;
}