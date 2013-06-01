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
using namespace Crimild::GL3;

/*

class ImageEffect {
public:
	ImageEffect( void );
	virtual ~ImageEffect( void );

	void setEnabled( bool value ) { _enabled = value; }
	bool isEnabled( void ) const { return _enabled; }

	void setProgram( ShaderProgramPtr program ) { _program = program; }
	ShaderProgram *getProgram( void ) { return _program; }

	Primitive *getScreenPrimitive( void ) { return _screenPrimitive; }

	virtual void render( Renderer *renderer, Texture *input )
	{
		renderer->bindProgram( getProgram() );
		renderer->bindTexture( getProgram()->getStandardLocation( ShaderProgram::StandardLocations::INPUT_SAMPLER_UNIFORM ), input );

		renderer->drawPrimitive( getScreenPrimitive() );

		renderer->unbindTexture( getProgram()->getStandardLocation( ShaderProgram::StandardLocations::INPUT_SAMPLER_UNIFORM ), input );
		renderer->unbindProgram( getProgram() );
	}

private:
	bool _enabled;
	PrimitivePtr _screenPrimitive;
	ShaderProgramPtr _program;
};

class BlurImageEffect {
public:
	BlurImageEffect( void )
		: _screen( new QuadPrimitive( 1.0, 1.0 ) )
	{

	}

	virtual void render( Renderer *renderer, Texture *colorMap )
	{
		renderer->bindProgram( _program );
		renderer->bindTexture( _program, colorMap );

		for ( int i = 0; i < 2; i++ ) {
			renderer->bindUniform( _program->getLocation( "pass" ), i )
			renderer->drawPrimitive( _screen );
		}

		renderer->unbindTexture( _program, colorMap );
		renderer->unbindProgram( _program );
	}

private:
	PrimitivePtr _screen;
};

void OffscreenRenderPass::render( Renderer *renderer, VisibilitySet *vs, Camera *camera ) 
{
	renderer->bindFrameBuffer( _offscreenFrameBuffer.get() );
	renderer->clearBuffers();
	RenderPass::render( renderer, vs, camera );
	renderer->unbindFrameBuffer( _offscreenFrameBuffer.get() );

	if ( shallApplyResults() ) {
		renderScreenBuffer( renderer );
	}
}

void OffscreenRenderPass::renderScreenBuffer( Renderer *renderer ) 
{
	renderer->clearBuffers();
	renderer->render( _screenPrimitive );
}

void OffscreenRenderPass::applyImageEffects( Renderer *renderer )
{
	if ( _postEffects.size() > 0 ) {
		renderer->bindFrameBuffer( getOffscreenBuffer() );
		for ( auto effect : _postEffects ) {
			if ( effect->isEnabled() ) {
				Texture *texture = getOffscreenBuffer()->getTexture();
				renderer->clearBuffers();
				effect->render( renderer, texture );
			}
		}
		renderer->unbindFrameBuffer( getOffscreenBuffer() );
	}
}

void OffscreenRenderPass::apply( Renderer *renderer )
{

}
*/

int main( int argc, char **argv )
{
	SimulationPtr sim( new GLSimulation( "Image effects", argc, argv ) );
	
	GroupPtr scene( new Group() );

	GeometryPtr geometry( new Geometry() );
	PrimitivePtr primitive( new NewellTeapotPrimitive() );
	geometry->attachPrimitive( primitive );
	RotationComponentPtr rotationComponent( new RotationComponent( Vector3f( 0, 1, 0 ), 0.25 ) );
	geometry->attachComponent( rotationComponent );
	scene->attachNode( geometry );

	LightPtr light( new Light() );
	light->local().setTranslate( -10.0f, 20.0f, 30.0f );
	scene->attachNode( light );

	CameraPtr camera( new Camera() );
	camera->local().setTranslate( 0.0f, 15.0f, 80.0f );
	scene->attachNode( camera );
	
	OffscreenRenderPassPtr renderPass( new OffscreenRenderPass() );
	camera->setRenderPass( renderPass );
	/*
	RenderPass *renderPass = camera->getRenderPass();
	SepiaToneEffectPtr sepiaTone( new SepiaToneEffect() ); 
	renderPass->attachImageEffect( sepiaTone );
	NoiseEffectPtr noiseEffect( new NoiseEffect() ); 
	renderPass->attachImageEffect( noiseEffect );
	ScratchEffectPtr scratchEffect( new ScratchEffect() ); 
	renderPass->attachImageEffect( scratchEffect );
	*/

	sim->attachScene( scene );
	return sim->run();
}

