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
#include "Library/ColorShaderProgram.hpp"
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

	MaterialPtr material( new Material() );
	material->setDiffuse( RGBAColorf( 1.0f, 0.0f, 1.0f, 1.0f ) );
	setDefaultMaterial( material );

	_fallbackPrograms[ "flat" ] = ShaderProgramPtr( new FlatShaderProgram() );
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

void GL3::Renderer::enableMaterialProperties( ShaderProgram *program, Material *material )
{
	ShaderLocation *materialDiffuseLocation = program->getMaterialDiffuseUniformLocation();
	if ( materialDiffuseLocation && materialDiffuseLocation->isValid() ) {
		glUniform4fv( materialDiffuseLocation->getLocation(), 1, static_cast< const float * >( material->getDiffuse() ) );
	}
}

void GL3::Renderer::applyTransformations( ShaderProgram *program, GeometryNode *geometry, Camera *camera )
{
	ShaderLocation *projMatrixLocation = program->getProjectionMatrixUniformLocation();
	if ( projMatrixLocation && projMatrixLocation->isValid() ) {
		glUniformMatrix4fv( projMatrixLocation->getLocation(), 1, GL_FALSE, static_cast< const float * >( camera->getProjectionMatrix() ) );
	}

	ShaderLocation *viewMatrixLocation = program->getViewMatrixUniformLocation();
	if ( viewMatrixLocation && viewMatrixLocation->isValid() ) {
		glUniformMatrix4fv( viewMatrixLocation->getLocation(), 1, GL_FALSE, static_cast< const GLfloat * >( camera->getViewMatrix() ) );
	}

	ShaderLocation *modelMatrixLocation = program->getModelMatrixUniformLocation();
	if ( modelMatrixLocation && modelMatrixLocation->isValid() ) {
		Matrix4f modelMatrix = geometry->getWorld().computeModelMatrix();
		glUniformMatrix4fv( modelMatrixLocation->getLocation(), 1, GL_FALSE, static_cast< const GLfloat * >( modelMatrix ) );
	}
}

void GL3::Renderer::drawPrimitive( ShaderProgram *program, Primitive *primitive )
{
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

void GL3::Renderer::restoreTransformations( ShaderProgram *program, GeometryNode *geometry, Camera *camera )
{

}

void GL3::Renderer::disableMaterialProperties( ShaderProgram *program, Material *material )
{

}

ShaderProgram *GL3::Renderer::getFallbackProgram( Material *material, Primitive *primitive )
{
	if ( material->getColorMap() ) {
		return _fallbackPrograms[ "texture" ].get();
	}

	if ( primitive->getVertexBuffer()->getVertexFormat().hasColors() ) {
		return _fallbackPrograms[ "color" ].get();
	}

	return _fallbackPrograms[ "flat" ].get();
}

