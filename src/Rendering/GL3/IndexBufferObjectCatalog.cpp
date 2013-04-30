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

#include "IndexBufferObjectCatalog.hpp"

#include <GL/glfw.h>

using namespace Crimild;

GL3::IndexBufferObjectCatalog::IndexBufferObjectCatalog( void )
{

}

GL3::IndexBufferObjectCatalog::~IndexBufferObjectCatalog( void )
{

}

int GL3::IndexBufferObjectCatalog::getNextResourceId( void )
{
    GLuint id;
    glGenBuffers( 1, &id );
    return id;
}

void GL3::IndexBufferObjectCatalog::bind( ShaderProgram *program, IndexBufferObject *ibo )
{
	Catalog< IndexBufferObject >::bind( program, ibo );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo->getCatalogId() );
}

void GL3::IndexBufferObjectCatalog::unbind( ShaderProgram *program, IndexBufferObject *ibo )
{
	Catalog< IndexBufferObject >::unbind( program, ibo );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void GL3::IndexBufferObjectCatalog::load( IndexBufferObject *ibo )
{
	Catalog< IndexBufferObject >::load( ibo );

	int id = ibo->getCatalogId();
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, id );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 
		sizeof( unsigned short ) * ibo->getIndexCount(), 
		ibo->getData(), 
		GL_STATIC_DRAW );
}

void GL3::IndexBufferObjectCatalog::unload( IndexBufferObject *ibo )
{
	GLuint bufferId = ibo->getCatalogId();
	glDeleteBuffers( 1, &bufferId );

	Catalog< IndexBufferObject >::unload( ibo );
}

