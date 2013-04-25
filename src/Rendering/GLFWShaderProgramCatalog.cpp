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

#include "GLFWShaderProgramCatalog.hpp"

#include "Exceptions/GLFWException.hpp"
#include "GLFWUtils.hpp"

#include <GL/glfw.h>

using namespace Crimild;

GLFWShaderProgramCatalog::GLFWShaderProgramCatalog( void )
{

}

GLFWShaderProgramCatalog::~GLFWShaderProgramCatalog( void )
{

}

int GLFWShaderProgramCatalog::getNextResourceId( void )
{
	return glCreateProgram();
}

void GLFWShaderProgramCatalog::bind( ShaderProgram *program )
{
	Catalog< ShaderProgram >::bind( program );

	glUseProgram( program->getCatalogId() );
}

void GLFWShaderProgramCatalog::unbind( ShaderProgram *program )
{
	Catalog< ShaderProgram >::unbind( program );

	glUseProgram( 0 );
}

void GLFWShaderProgramCatalog::load( ShaderProgram *program )
{
	Catalog< ShaderProgram >::load( program );

	int programId = program->getCatalogId();
	if ( programId > 0 ) {
		int vsId = compileShader( program->getVertexShader(), GL_VERTEX_SHADER );
		int fsId = compileShader( program->getFragmentShader(), GL_FRAGMENT_SHADER );

        if ( vsId > 0 && fsId > 0 ) {
    		glAttachShader( programId, vsId );
    		glAttachShader( programId, fsId );

            glLinkProgram( programId );

            glDetachShader( programId, vsId );
            glDeleteShader( vsId );

            glDetachShader( programId, fsId );
            glDeleteShader( fsId );

            GLint linkStatus = GL_FALSE;
            glGetProgramiv( programId, GL_LINK_STATUS, &linkStatus );
            if ( linkStatus == GL_FALSE ) {
                GLint bufLength = 0;
                glGetProgramiv( programId, GL_INFO_LOG_LENGTH, &bufLength );
                if ( bufLength ) {
                    char *buf = ( char * ) malloc( bufLength );
                    if ( buf ) {
                        glGetProgramInfoLog( programId, bufLength, NULL, buf );
                        std::string message( buf );
                        free( buf );
                        throw GLFWException( "Could not link program: " + message );
                    }
                }

                glDeleteProgram( programId );
                programId = 0;
            }

            program->foreachLocation( [&]( ShaderLocationPtr &loc ) mutable {
            	if ( loc->getType() == ShaderLocation::Type::ATTRIBUTE ) {
            		fetchAttributeLocation( program, loc.get() );
            	}
            	else {
            		fetchUniformLocation( program, loc.get() );
            	}
            });
        }
	}
}

void GLFWShaderProgramCatalog::unload( ShaderProgram *program )
{
	int programId = program->getCatalogId();
	if ( programId > 0 ) {
		glDeleteProgram( programId );
	}

	Catalog< ShaderProgram >::unload( program );
}

int GLFWShaderProgramCatalog::compileShader( Shader *shader, int type )
{
	GLuint shaderId = glCreateShader( type );
    if ( shaderId > 0 ) {
		const char *source = shader->getSource();
		glShaderSource( shaderId, 1, ( const GLchar ** ) &source, NULL );
        glCompileShader( shaderId );

        GLint compiled = GL_FALSE;
        glGetShaderiv( shaderId, GL_COMPILE_STATUS, &compiled );
    
        if ( compiled == GL_FALSE ) {
            GLint infoLen = 0;
            glGetShaderiv( shaderId, GL_INFO_LOG_LENGTH, &infoLen );
            if ( infoLen ) {
                char* buf = ( char * ) malloc( infoLen );
				if ( buf ) {
                    glGetShaderInfoLog( shaderId, infoLen, NULL, buf );
                    std::string message( buf );
                    free( buf );
                    std::cout << std::string( "Could not compile " ) + ( type == GL_VERTEX_SHADER ? "vertex" : "fragment" ) + " shader: " + message << std::endl;
                    throw GLFWException( std::string( "Could not compile " ) + ( type == GL_VERTEX_SHADER ? "vertex" : "fragment" ) + " shader: " + message );
                }
            }

            glDeleteShader( shaderId );
            shaderId = 0;
        }
    }

    return shaderId;
}

void GLFWShaderProgramCatalog::fetchAttributeLocation( ShaderProgram *program, ShaderLocation *location )
{
	location->setLocation( glGetAttribLocation( program->getCatalogId(), location->getName().c_str() ) );
}

void GLFWShaderProgramCatalog::fetchUniformLocation( ShaderProgram *program, ShaderLocation *location )
{
	location->setLocation( glGetUniformLocation( program->getCatalogId(), location->getName().c_str() ) );
}

