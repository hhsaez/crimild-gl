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
	FileSystem::getInstance().init( argc, argv );

	float vertices[] = {
		-1.0f, +1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		+1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		+1.0f, +1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f
	};

	unsigned short indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	PrimitivePtr primitive( new Primitive( Primitive::Type::TRIANGLES ) );
	primitive->setVertexBuffer( VertexBufferObjectPtr( new VertexBufferObject( VertexFormat::VF_P3_N3_UV2, 4, vertices ) ) );
	primitive->setIndexBuffer( IndexBufferObjectPtr( new IndexBufferObject( 6, indices ) ) );

	GeometryNodePtr geometry( new GeometryNode() );
	geometry->attachPrimitive( primitive );
	geometry->local().setTranslate( 0.0f, 0.0f, -3.0f );

	MaterialPtr material( new Material() );
	ImagePtr image( new ImageTGA( FileSystem::getInstance().pathForResource( "bricks.tga" ) ) );
	TexturePtr texture( new Texture( image ) );
	material->setColorMap( texture );
	MaterialComponentPtr materials( new MaterialComponent() );
	materials->attachMaterial( material );
	geometry->attachComponent( materials );

	GroupNodePtr scene( new GroupNode() );
	scene->attachNode( geometry );

	CameraNodePtr camera( new CameraNode() );
	scene->attachNode( camera );

	SimulationPtr sim( new GLSimulation( "Textures" ) );
	sim->attachScene( scene );
	return sim->run();
}
