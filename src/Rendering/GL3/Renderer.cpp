/*
 * Copyright (c) 2013, Hernan Saez
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Renderer.hpp"
#include "ShaderProgramCatalog.hpp"
#include "VertexBufferObjectCatalog.hpp"
#include "IndexBufferObjectCatalog.hpp"
#include "TextureCatalog.hpp"
#include "Library/FlatShaderProgram.hpp"
#include "Library/GouraudShaderProgram.hpp"
#include "Library/ColorShaderProgram.hpp"
#include "Library/PhongShaderProgram.hpp"
#include "Library/TextureShaderProgram.hpp"
#include "Utils.hpp"

#include <GL/glew.h>
#include <GL/glfw.h>

using namespace Crimild;

GL3::Renderer::Renderer( FrameBufferObjectPtr screenBuffer )
{
	setShaderProgramCatalog( ShaderProgramCatalogPtr( new GL3::ShaderProgramCatalog() ) );
	setVertexBufferObjectCatalog( VertexBufferObjectCatalogPtr( new GL3::VertexBufferObjectCatalog() ) );
	setIndexBufferObjectCatalog( IndexBufferObjectCatalogPtr( new GL3::IndexBufferObjectCatalog() ) );
	setTextureCatalog( TextureCatalogPtr( new GL3::TextureCatalog() ) );

	_fallbackPrograms[ "flat" ] = ShaderProgramPtr( new FlatShaderProgram() );
	_fallbackPrograms[ "gouraud" ] = ShaderProgramPtr( new GouraudShaderProgram() );
	_fallbackPrograms[ "phong" ] = ShaderProgramPtr( new PhongShaderProgram() );
	_fallbackPrograms[ "color" ] = ShaderProgramPtr( new ColorShaderProgram() );
	_fallbackPrograms[ "texture" ] = ShaderProgramPtr( new TextureShaderProgram() );

	setScreenBuffer( screenBuffer );
}

GL3::Renderer::~Renderer( void )
{

}

void GL3::Renderer::configure( void )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	Log::Info << "Configuring renderer"
    		  << "\n       OpenGL version: " << glGetString( GL_VERSION )
    		  << "\n       GLSL version: " << glGetString( GL_SHADING_LANGUAGE_VERSION )
    		  << "\n       Vendor: " << glGetString( GL_VENDOR )
    		  << "\n       Renderer: " << glGetString( GL_RENDERER )
    		  << Log::End;

	glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
	if ( glewInit() != GLEW_OK ) {
		Log::Fatal << "Cannot initialize GLEW" << Log::End;
		exit( 1 );
	}

	if ( !GLEW_VERSION_3_2 ) {
		Log::Fatal << "OpenGL 3.2 API is not available" << Log::End;
		exit( 1 );
    }

    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void GL3::Renderer::beginRender( void )
{

}

void GL3::Renderer::endRender( void )
{

}

void GL3::Renderer::clearBuffers( void )
{
	const RGBAColorf &clearColor = getScreenBuffer()->getClearColor();
	glClearColor( clearColor.r(), clearColor.g(), clearColor.b(), clearColor.a() );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void GL3::Renderer::bindUniform( ShaderLocation *location, int value ) 
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	if ( location && location->isValid() ) {
		glUniform1i( location->getLocation(), value );
	}

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void GL3::Renderer::bindUniform( ShaderLocation *location, float value ) 
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	if ( location && location->isValid() ) {
		glUniform1f( location->getLocation(), value );
	}

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void GL3::Renderer::bindUniform( ShaderLocation *location, const Vector3f &vector ) 
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	if ( location && location->isValid() ) {
		glUniform3fv( location->getLocation(), 1, static_cast< const GLfloat * >( vector.getData() ) );
	}

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void GL3::Renderer::bindUniform( ShaderLocation *location, const RGBAColorf &color ) 
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	if ( location && location->isValid() ) {
		glUniform4fv( location->getLocation(), 1, static_cast< const GLfloat * >( color.getData() ) );
	}

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void GL3::Renderer::bindUniform( ShaderLocation *location, const Matrix4f &matrix ) 
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	if ( location && location->isValid() ) {
		glUniformMatrix4fv( location->getLocation(), 1, GL_FALSE, static_cast< const GLfloat * >( matrix.getData() ) );
	}

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void GL3::Renderer::drawPrimitive( ShaderProgram *program, Primitive *primitive )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	GLenum type;
	switch ( primitive->getType() ) {
		case Primitive::Type::POINTS:
			type = GL_POINTS;
			break;

		case Primitive::Type::LINES:
			type = GL_LINES;
			break;
			
		case Primitive::Type::LINE_LOOP:
			type = GL_LINE_LOOP;
			break;
			
		case Primitive::Type::LINE_STRIP:
			type = GL_LINE_STRIP;
			break;
			
		case Primitive::Type::TRIANGLE_FAN:
			type = GL_TRIANGLE_FAN;
			break;
			
		case Primitive::Type::TRIANGLE_STRIP:
			type = GL_TRIANGLE_STRIP;
			break;
			
		case Primitive::Type::TRIANGLES:
		default:
			type = GL_TRIANGLES;
			break;
	}

	unsigned short *base = 0;
	glDrawElements( type,
				   primitive->getIndexBuffer()->getIndexCount(),
				   GL_UNSIGNED_SHORT,
				   ( const GLvoid * ) base );
	

	CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

ShaderProgram *GL3::Renderer::getFallbackProgram( Material *material, Geometry *geometry, Primitive *primitive )
{
	if ( material->getColorMap() ) {
		return _fallbackPrograms[ "texture" ].get();
	}

	if ( geometry->getComponent< RenderStateComponent >()->hasLights() ) {
		return _fallbackPrograms[ "phong" ].get();
	}

	if ( primitive->getVertexBuffer()->getVertexFormat().hasColors() ) {
		return _fallbackPrograms[ "color" ].get();
	}

	return _fallbackPrograms[ "flat" ].get();
}

