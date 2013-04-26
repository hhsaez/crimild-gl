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

#include "GLFWRenderer.hpp"
#include "GLFWShaderProgramCatalog.hpp"
#include "GLFWVertexBufferObjectCatalog.hpp"
#include "GLFWIndexBufferObjectCatalog.hpp"
#include "GLFWTextureCatalog.hpp"
#include "ShaderLibrary/SimpleShaderProgram.hpp"
#include "ShaderLibrary/TextureShaderProgram.hpp"
#include "GLFWUtils.hpp"

#include <GL/glfw.h>

using namespace Crimild;

GLFWRenderer::GLFWRenderer( FrameBufferObjectPtr screenBuffer )
{
	setShaderProgramCatalog( ShaderProgramCatalogPtr( new GLFWShaderProgramCatalog() ) );
	setVertexBufferObjectCatalog( VertexBufferObjectCatalogPtr( new GLFWVertexBufferObjectCatalog() ) );
	setIndexBufferObjectCatalog( IndexBufferObjectCatalogPtr( new GLFWIndexBufferObjectCatalog() ) );
	setTextureCatalog( TextureCatalogPtr( new GLFWTextureCatalog() ) );

	MaterialPtr material( new Material() );
	ShaderProgramPtr program( new SimpleShaderProgram() );
	material->setProgram( program );
	setDefaultMaterial( material );

	_fallbackPrograms[ "simple" ] = ShaderProgramPtr( new SimpleShaderProgram() );
	_fallbackPrograms[ "texture" ] = ShaderProgramPtr( new TextureShaderProgram() );

	setScreenBuffer( screenBuffer );
}

GLFWRenderer::~GLFWRenderer( void )
{

}

void GLFWRenderer::configure( void )
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void GLFWRenderer::beginRender( void )
{

}

void GLFWRenderer::endRender( void )
{

}

void GLFWRenderer::clearBuffers( void )
{
	const RGBAColorf &clearColor = getScreenBuffer()->getClearColor();
	glClearColor( clearColor.r(), clearColor.g(), clearColor.b(), clearColor.a() );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void GLFWRenderer::applyTransformations( ShaderProgram *program, GeometryNode *geometry, Camera *camera )
{
	ShaderLocation *projMatrixLocation = program->getLocation( ShaderLocation::DefaultLocations::PROJECTION_MATRIX_UNIFORM_NAME );
	if ( projMatrixLocation->isValid() ) {
		glUniformMatrix4fv( projMatrixLocation->getLocation(), 1, GL_FALSE, static_cast< const float * >( camera->getProjectionMatrix() ) );
	}

	ShaderLocation *viewMatrixLocation = program->getLocation( ShaderLocation::DefaultLocations::VIEW_MATRIX_UNIFORM_NAME );
	if ( viewMatrixLocation->isValid() ) {
		glUniformMatrix4fv( viewMatrixLocation->getLocation(), 1, GL_FALSE, static_cast< const GLfloat * >( camera->getViewMatrix() ) );
	}

	ShaderLocation *modelMatrixLocation = program->getLocation( ShaderLocation::DefaultLocations::MODEL_MATRIX_UNIFORM_NAME );
	if ( modelMatrixLocation->isValid() ) {
		Matrix4f modelMatrix = geometry->getWorld().computeModelMatrix();
		glUniformMatrix4fv( modelMatrixLocation->getLocation(), 1, GL_FALSE, static_cast< const GLfloat * >( modelMatrix ) );
	}
}

void GLFWRenderer::drawPrimitive( ShaderProgram *program, Primitive *primitive )
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
	

	CRIMILD_CHECK_GL_ERRORS_AFTER( GLFWRenderer::drawPrimitive );
}

void GLFWRenderer::restoreTransformations( ShaderProgram *program, GeometryNode *geometry, Camera *camera )
{

}

ShaderProgram *GLFWRenderer::getFallbackProgram( Material *material )
{
	if ( material->getColorMap() ) {
		return _fallbackPrograms[ "texture" ].get();
	}

	return _fallbackPrograms[ "simple" ].get();
}

