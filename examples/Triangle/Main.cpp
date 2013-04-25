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

#include <Crimild.hpp>
#include <CrimildGL.hpp>

using namespace Crimild;

int main( int argc, char **argv )
{
	float vertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0
	};

	unsigned short indices[] = {
		0, 1, 2
	};

	VertexBufferObjectPtr vbo( new VertexBufferObject( VertexFormat::VF_P3, 3, vertices ) );
	IndexBufferObjectPtr ibo( new IndexBufferObject( 3, indices ) );
	
	PrimitivePtr primitive( new Primitive( Primitive::Type::TRIANGLES ) );
	primitive->setVertexBuffer( vbo );
	primitive->setIndexBuffer( ibo );

	GeometryNodePtr geometry( new GeometryNode() );
	geometry->attachPrimitive( primitive );

	geometry->local().setTranslate( 0, 0, -3 );
	RotationComponentPtr rotationComponent( new RotationComponent( Vector3f( 0, 1, 0 ), 0.5 ) );
	geometry->attachComponent( rotationComponent );

	GroupNodePtr scene( new GroupNode() );
	scene->attachNode( geometry );

	CameraNodePtr camera( new CameraNode() );
	scene->attachNode( camera );

	SimulationPtr sim( new GLSimulation( "test" ) );
	sim->attachScene( scene );
	return sim->run();
}

