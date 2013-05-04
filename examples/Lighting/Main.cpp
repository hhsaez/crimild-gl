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
	SimulationPtr sim( new GLSimulation( "Lighting", argc, argv ) );

	std::cout << "Press 1 to use Phong shading (default)\n"
		  	  << "Press 2 to use Gouraud shading" << std::endl;

	GeometryPtr trefoilKnot( new Geometry() );
	PrimitivePtr trefoilKnotPrimitive( new TrefoilKnotPrimitive( Primitive::Type::TRIANGLES, 1.0, VertexFormat::VF_P3_N3 ) );
	trefoilKnot->attachPrimitive( trefoilKnotPrimitive );
	
	NodeComponentPtr rotate( new RotationComponent( Vector3f( 0.0f, 1.0f, 0.0f ), 0.1f ) );
	trefoilKnot->attachComponent( rotate );

	MaterialPtr phongMaterial( new GL3::PhongMaterial() );
	MaterialPtr gouraudMaterial( new GL3::GouraudMaterial() );
	MaterialComponent *materials = trefoilKnot->getComponent< MaterialComponent >();

	NodeComponentPtr changeMaterial( new LambdaComponent( [&]( Node *node, const Time & ) {
		if ( InputState::getCurrentState().isKeyDown( '1' ) ) {
			materials->detachAllMaterials();
			materials->attachMaterial( phongMaterial );
			node->perform( UpdateRenderState() );
		}
		else if ( InputState::getCurrentState().isKeyDown( '2' ) ) {
			materials->detachAllMaterials();
			materials->attachMaterial( gouraudMaterial );
			node->perform( UpdateRenderState() );
		}
	}));
	trefoilKnot->attachComponent( changeMaterial );
	
	GroupPtr scene( new Group() );
	scene->attachNode( trefoilKnot );

	LightPtr light( new Light() );
	light->local().setTranslate( 0.0f, 1.0f, 3.0f );
	scene->attachNode( light );

	CameraPtr camera( new Camera() );
	camera->local().setTranslate( 0.0f, 0.0f, 1.0f );
	scene->attachNode( camera );

	sim->attachScene( scene );
	return sim->run();
}

