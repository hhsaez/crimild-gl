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

#include "PhongShaderProgram.hpp"
#include "Rendering/GL3/Utils.hpp"

using namespace Crimild;
using namespace Crimild::GL3;

const char *phong_vs = { CRIMILD_TO_STRING(
	in vec3 aPosition;
	in vec3 aNormal;

	uniform mat4 uPMatrix;
	uniform mat4 uVMatrix;
	uniform mat4 uMMatrix;

	out vec4 vWorldVertex;
	out vec3 vWorldNormal;
	out vec3 vViewVec;

	void main ()
	{
	    vWorldVertex = uMMatrix * vec4( aPosition, 1.0 );
	    vec4 viewVertex = uVMatrix * vWorldVertex;
	    gl_Position = uPMatrix * viewVertex;
	    vWorldNormal = normalize( mat3( uMMatrix ) * aNormal );
	    vViewVec = normalize( -viewVertex.xyz );
	}
)};

const char *phong_fs = { CRIMILD_TO_STRING( 
	struct Light {
	    vec3 position;
	    vec3 attenuation;
	    vec3 direction;
	    vec4 color;
	    float outerCutoff;
	    float innerCutoff;
	    float exponent;
	};

	struct Material {
	    vec4 ambient;
	    vec4 diffuse;
	    vec4 specular;
	    float shininess;
	};

	in vec4 vWorldVertex;
	in vec3 vWorldNormal;
	in vec3 vViewVec;

	uniform int uLightCount;
	uniform Light uLights[ 4 ];
	uniform Material uMaterial;

	out vec4 vFragColor;

	void main( void ) 
	{ 
        vFragColor = uMaterial.ambient;
        for ( int i = 0; i < 4; i++ ) {
            if ( i >= uLightCount ) {
                break;
            }
            
            vec3 lightVec = normalize( uLights[ i ].position - vWorldVertex.xyz );
            float l = dot( vWorldNormal, lightVec );
            if ( l > 0.0 ) {
                float spotlight = 1.0;
                if ( ( uLights[ i ].direction.x != 0.0 ) || ( uLights[ i ].direction.y != 0.0 ) || ( uLights[ i ].direction.z != 0.0 ) ) {
                    spotlight = max( -dot( lightVec, uLights[ i ].direction ), 0.0 );
                    float spotlightFade = clamp( ( uLights[ i ].outerCutoff - spotlight ) / ( uLights[ i ].outerCutoff - uLights[ i ].innerCutoff ), 0.0, 1.0 );
                    spotlight = pow( spotlight * spotlightFade, uLights[ i ].exponent );
                }
                
                vec3 r = -normalize( reflect( lightVec, vWorldNormal ) );
                float s = pow( max( dot( r, vViewVec ), 0.0 ), uMaterial.shininess );
                
                float d = distance( vWorldVertex.xyz, uLights[ i ].position );
                float a = 1.0 / ( uLights[ i ].attenuation.x + ( uLights[ i ].attenuation.y * d ) + ( uLights[ i ].attenuation.z * d * d ) );
                
                vFragColor.xyz += ( ( uMaterial.diffuse.xyz * l ) + ( uMaterial.specular.xyz * s ) ) * uLights[ i ].color.xyz * a * spotlight;
            }
        }
        
        vFragColor.w = uMaterial.diffuse.w;
	}
)};

PhongShaderProgram::PhongShaderProgram( void )
	: ShaderProgram( Utils::getVertexShaderInstance( phong_vs ), Utils::getFragmentShaderInstance( phong_fs ) )
{ 
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::POSITION_ATTRIBUTE, "aPosition" );
	registerStandardLocation( ShaderLocation::Type::ATTRIBUTE, ShaderProgram::StandardLocation::NORMAL_ATTRIBUTE, "aNormal" );

	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM, "uPMatrix" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM, "uVMatrix" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM, "uMMatrix" );

	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_AMBIENT_UNIFORM, "uMaterial.ambient" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_DIFFUSE_UNIFORM, "uMaterial.diffuse" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_SPECULAR_UNIFORM, "uMaterial.specular" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_SHININESS_UNIFORM, "uMaterial.shininess" );

	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_COUNT_UNIFORM, "uLightCount" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_POSITION_UNIFORM, "uLights[0].position" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_ATTENUATION_UNIFORM, "uLights[0].attenuation" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_DIRECTION_UNIFORM, "uLights[0].direction" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_COLOR_UNIFORM, "uLights[0].color" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_OUTER_CUTOFF_UNIFORM, "uLights[0].outerCutoff" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_INNER_CUTOFF_UNIFORM, "uLights[0].innerCutoff" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::LIGHT_EXPONENT_UNIFORM, "uLights[0].exponent" );
}

PhongShaderProgram::~PhongShaderProgram( void )
{ 
}

