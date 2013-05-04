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

#include "VertexBufferObjectCatalog.hpp"
#include "Utils.hpp"

#include <GL/glew.h>
#include <GL/glfw.h>

using namespace Crimild;

GL3::VertexBufferObjectCatalog::VertexBufferObjectCatalog( void )
{

}

GL3::VertexBufferObjectCatalog::~VertexBufferObjectCatalog( void )
{

}

int GL3::VertexBufferObjectCatalog::getNextResourceId( void )
{
	int compositeId;

	GLuint vaoId;
	glGenVertexArrays( 1, &vaoId );

	glBindVertexArray( vaoId );

	GLuint vboId;    
    glGenBuffers( 1, &vboId );

    return composeId( vaoId, vboId );
}

int GL3::VertexBufferObjectCatalog::composeId( unsigned int vaoId, unsigned int vboId )
{
	return vaoId * 1000 + vboId;
}

bool GL3::VertexBufferObjectCatalog::extractId( int compositeId, unsigned int &vaoId, unsigned int &vboId )
{
	vaoId = compositeId / 1000;
	vboId = compositeId % 1000;
	return true;
}

void GL3::VertexBufferObjectCatalog::bind( ShaderProgram *program, VertexBufferObject *vbo )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	GLuint vaoId, vboId;

	if ( vbo->getCatalog() == nullptr ) {
		Catalog< VertexBufferObject >::bind( program, vbo );		

		extractId( vbo->getCatalogId(), vaoId, vboId );

		glBindVertexArray( vaoId );

	    glBindBuffer( GL_ARRAY_BUFFER, vboId );
	    float *baseOffset = 0;

	    const VertexFormat &format = vbo->getVertexFormat();

	    ShaderLocation *positionLocation = program->getStandardLocation( ShaderProgram::StandardLocation::POSITION_ATTRIBUTE );
	    if ( positionLocation && positionLocation->isValid() ) {
	        if ( format.hasPositions() ) {
	            glEnableVertexAttribArray( positionLocation->getLocation() );
	            glVertexAttribPointer( positionLocation->getLocation(),
	                                   format.getPositionComponents(),
	                                   GL_FLOAT,
	                                   GL_FALSE,
	                                   format.getVertexSizeInBytes(),
	                                   ( const GLvoid * )( baseOffset + format.getPositionsOffset() ) );
	        }
	    }

	    ShaderLocation *normalLocation = program->getStandardLocation( ShaderProgram::StandardLocation::NORMAL_ATTRIBUTE );
	    if ( normalLocation && normalLocation->isValid() ) {
	        if ( format.hasPositions() ) {
	            glEnableVertexAttribArray( normalLocation->getLocation() );
	            glVertexAttribPointer( normalLocation->getLocation(),
	                                   format.getNormalComponents(),
	                                   GL_FLOAT,
	                                   GL_FALSE,
	                                   format.getVertexSizeInBytes(),
	                                   ( const GLvoid * )( baseOffset + format.getNormalsOffset() ) );
	        }
	    }

	    ShaderLocation *colorLocation = program->getStandardLocation( ShaderProgram::StandardLocation::COLOR_ATTRIBUTE );
	    if ( colorLocation && colorLocation->isValid() ) {
	        if ( format.hasColors() ) {
	            glEnableVertexAttribArray( colorLocation->getLocation() );
	            glVertexAttribPointer( colorLocation->getLocation(),
	                                   format.getColorComponents(),
	                                   GL_FLOAT,
	                                   GL_FALSE,
	                                   format.getVertexSizeInBytes(),
	                                   ( const GLvoid * )( baseOffset + format.getColorsOffset() ) );
	        }
	    }

	    ShaderLocation *uvLocation = program->getStandardLocation( ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE );
	    if ( uvLocation && uvLocation->isValid() ) {
	        if ( format.hasTextureCoords() ) {
	            glEnableVertexAttribArray( uvLocation->getLocation() );
	            glVertexAttribPointer( uvLocation->getLocation(),
	                                   format.getTextureCoordComponents(),
	                                   GL_FLOAT,
	                                   GL_FALSE,
	                                   format.getVertexSizeInBytes(),
	                                   ( const GLvoid * )( baseOffset + format.getTextureCoordsOffset() ) );
	        }
	    }
	}

	extractId( vbo->getCatalogId(), vaoId, vboId );
	glBindVertexArray( vaoId );

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void GL3::VertexBufferObjectCatalog::unbind( ShaderProgram *program, VertexBufferObject *vbo )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

    glBindVertexArray( 0 );
    
	Catalog< VertexBufferObject >::unbind( program, vbo );

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void GL3::VertexBufferObjectCatalog::load( VertexBufferObject *vbo )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	Catalog< VertexBufferObject >::load( vbo );

	GLuint vaoId, vboId;
	extractId( vbo->getCatalogId(), vaoId, vboId );

	glBindVertexArray( vaoId );	

    glBindBuffer( GL_ARRAY_BUFFER, vboId );
    glBufferData( GL_ARRAY_BUFFER,
         vbo->getVertexFormat().getVertexSizeInBytes() * vbo->getVertexCount(),
         vbo->getData(),
         GL_STATIC_DRAW );

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

void GL3::VertexBufferObjectCatalog::unload( VertexBufferObject *vbo )
{
	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	GLuint vaoId, vboId;
	extractId( vbo->getCatalogId(), vaoId, vboId );

    glDeleteBuffers( 1, &vboId );
	glDeleteVertexArrays( 1, &vaoId );

	Catalog< VertexBufferObject >::unload( vbo );

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}

