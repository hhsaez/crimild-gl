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

#include "GLFWVertexBufferObjectCatalog.hpp"

#include "Exceptions/GLFWException.hpp"

#include <GL/glfw.h>

using namespace Crimild;

GLFWVertexBufferObjectCatalog::GLFWVertexBufferObjectCatalog( void )
{

}

GLFWVertexBufferObjectCatalog::~GLFWVertexBufferObjectCatalog( void )
{

}

int GLFWVertexBufferObjectCatalog::getNextResourceId( void )
{
	GLuint vboId;    
    glGenBuffers( 1, &vboId );
    return vboId;
}

void GLFWVertexBufferObjectCatalog::bind( ShaderProgram *program, VertexBufferObject *vbo )
{
	Catalog< VertexBufferObject >::bind( program, vbo );

    glBindBuffer( GL_ARRAY_BUFFER, vbo->getCatalogId() );
    float *baseOffset = 0;

    const VertexFormat &format = vbo->getVertexFormat();

    ShaderLocation *positionLocation = program->getLocation( ShaderLocation::DefaultLocations::POSITION_ATTRIBUTE_NAME );
    if ( positionLocation->isValid() ) {
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

    ShaderLocation *uvLocation = program->getLocation( ShaderLocation::DefaultLocations::TEXTURE_COORDS_ATTRIBUTE_NAME );
    if ( uvLocation->isValid() ) {
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

void GLFWVertexBufferObjectCatalog::unbind( ShaderProgram *program, VertexBufferObject *vbo )
{
    ShaderLocation *positionLocation = program->getLocation( ShaderLocation::DefaultLocations::POSITION_ATTRIBUTE_NAME );
    if ( positionLocation->isValid() ) {
        glDisableVertexAttribArray( positionLocation->getLocation() );
    }

    ShaderLocation *uvLocation = program->getLocation( ShaderLocation::DefaultLocations::TEXTURE_COORDS_ATTRIBUTE_NAME );
    if ( uvLocation->isValid() ) {
        glDisableVertexAttribArray( uvLocation->getLocation() );
    }

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    
	Catalog< VertexBufferObject >::unbind( program, vbo );
}

void GLFWVertexBufferObjectCatalog::load( VertexBufferObject *vbo )
{
	Catalog< VertexBufferObject >::load( vbo );

    int id = vbo->getCatalogId();
    glBindBuffer( GL_ARRAY_BUFFER, id );
    glBufferData( GL_ARRAY_BUFFER,
         vbo->getVertexFormat().getVertexSizeInBytes() * vbo->getVertexCount(),
         vbo->getData(),
         GL_STATIC_DRAW );
}

void GLFWVertexBufferObjectCatalog::unload( VertexBufferObject *vbo )
{
    GLuint bufferId = vbo->getCatalogId();
    glDeleteBuffers( 1, &bufferId );

	Catalog< VertexBufferObject >::unload( vbo );
}

