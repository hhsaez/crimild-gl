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

#include "Utils.hpp"

#include <GL/glew.h>
#include <GL/glfw.h>

using namespace Crimild;

void GL3::Utils::checkErrors( std::string prefix )
{
    for ( GLint error = glGetError(); error; error = glGetError() ) {

    	std::string errorDescription;
    	switch (error) {
			case GL_INVALID_ENUM:
				errorDescription += "GL_INVALID_ENUM";
				break;

			case GL_INVALID_VALUE:
				errorDescription += "GL_INVALID_VALUE";
				break;

			case GL_INVALID_OPERATION:
				errorDescription += "GL_INVALID_OPERATION";
				break;

			case GL_STACK_OVERFLOW:
				errorDescription += "GL_STACK_OVERFLOW";  
				break;

			case GL_STACK_UNDERFLOW: 
				errorDescription += "GL_STACK_UNDERFLOW";
				break;

			case GL_OUT_OF_MEMORY: 
				errorDescription += "GL_OUT_OF_MEMORY";
				break;

			default:
				errorDescription += "Unknown Error";
				break;
    	}

    	Log::Error << prefix << ": " << "(0x" << error << ") " << errorDescription << " " << glewGetErrorString( error ) << Log::End; 
    }
}

VertexShaderPtr GL3::Utils::getVertexShaderInstance( std::string source )
{
	VertexShaderPtr shader( new VertexShader( "#version 150\n" + source ) );
	return shader;
}

FragmentShaderPtr GL3::Utils::getFragmentShaderInstance( std::string source )
{
	FragmentShaderPtr shader( new FragmentShader( "#version 150\n" + source ) );
	return shader;
}

