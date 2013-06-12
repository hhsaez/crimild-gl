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

#include <fstream>
#include <string>
#include <vector>

using namespace Crimild;

#define OBJ_DATA_BUFFER_INITIAL_SIZE 1024
#define OBJ_DATA_BUFFER_GROW_BY 1024

class StringUtils {
public:
	template< typename T >
	static void toValue( std::string input, T &output )
	{
		std::stringstream str;
		str << input;
		str >> output;
	}

	template< typename T >
	static std::vector< T > split( std::string input, char delim )
	{
		std::vector< T > result;
		std::istringstream iss( input );
		while ( !iss.eof() ) {
			std::string str;
			getline( iss, str, delim );
			T value;
			toValue< T >( str, value );
			result.push_back( value );
		}
		return result;
	}
};

class OBJLoader {
public:
	OBJLoader( std::string filePath );
	virtual ~OBJLoader( void );

	NodePtr load( void );

private:
	void reset( void );
	void processLine( std::ifstream &input );
	void processMaterialFile( std::string materialFileName );

	template< typename T >
	void loadData( std::ifstream &input, std::vector< T > &storage, unsigned int &usedCount, unsigned int dataCount ) 
	{
		for ( int i = 0; i < dataCount; i++ ) {
			T value;
			input >> value; 
			pushData( storage, usedCount, value );
		}
	}

	template< typename T >
	void pushData( std::vector< T > &storage, unsigned int &usedCount, T data ) 
	{
		if ( usedCount >= storage.size() ) {
			// resize storage if needed, making sure there is place for at least
			// the number of items we're going to load
			storage.resize( storage.size() + OBJ_DATA_BUFFER_GROW_BY );
		}

		storage[ usedCount++ ] = data;
	}

	void pushGroup( std::string name, std::string materialName );

	NodePtr generateScene( void );

private:
	std::string _filePath;
	std::vector< float > _positions;
	unsigned int _positionCount;
	std::vector< float > _normals;
	unsigned int _normalCount;
	std::vector< float > _textureCoords;
	unsigned int _textureCoordCount;

	class GroupDef {
	public:
		std::string name;
		std::string materialName;
		std::vector< std::string > faces;

		GroupDef( std::string name, std::string materialName )
		{
			this->name = name;
			this->materialName = materialName;
		}
	};

	std::vector< std::shared_ptr< GroupDef >> _groups;
	GroupDef *_currentGroup;

	class MaterialDef {
	public:
		std::string name;

		TexturePtr diffuseMap;

		MaterialDef( std::string name )
		{
			this->name = name;
		}
	};	

	std::map< std::string, std::shared_ptr< MaterialDef >> _materials;
};

OBJLoader::OBJLoader( std::string filePath )
	: _filePath( filePath ),
	  _currentGroup( nullptr )
{
}

OBJLoader::~OBJLoader( void )
{

}

void OBJLoader::reset( void )
{
	_positionCount = 0;
	_normalCount = 0;
	_textureCoordCount = 0;

	std::vector< std::shared_ptr< GroupDef >> empty;
	_groups.swap( empty );
	pushGroup( _filePath, _filePath );

	_materials.clear();
}

NodePtr OBJLoader::load( void )
{
	Log::Debug << "Loading " << _filePath << Log::End;

	reset();

	std::ifstream input;
	input.open( _filePath.c_str() );
	if ( !input.is_open() ) {
		Log::Error << "Cannot find file " << _filePath << Log::End;
		return nullptr;
	}
	while ( !input.eof() ) {
		processLine( input );
	}

	return generateScene();
}

void OBJLoader::processLine( std::ifstream &input )
{
	std::string what;
	input >> what;

	if ( what == "v" ) {
		loadData( input, _positions, _positionCount, 3 );
	}
	else if ( what == "vn" ) {
		loadData( input, _normals, _normalCount, 3 );
	}
	else if ( what == "vt" ) {
		loadData( input, _textureCoords, _textureCoordCount, 2 );
	}
	else if ( what == "f" ) {
		// assumes a triangulated object
		std::string f0, f1, f2;
		input >> f0 >> f1 >> f2;
		_currentGroup->faces.push_back( f0 );
		_currentGroup->faces.push_back( f1 );
		_currentGroup->faces.push_back( f2 );
	}
	else if ( what == "mtllib" ) {
		std::string materialFileName;
		input >> materialFileName;
		processMaterialFile( FileSystem::getInstance().extractDirectory( _filePath ) + "/" + materialFileName );
	}
	else if ( what == "usemtl" ) {
		std::string name;
		input >> name;
		pushGroup( name, name );
	}
	else if ( what.length() > 0 ) {
		// unknown object. discard the line
		char buffer[ 1024 ];
		input.getline( buffer, 1024 );
		Log::Debug << "Skipping " << what << " " << buffer << Log::End;
	}
}

void OBJLoader::pushGroup( std::string name, std::string materialName )
{
	std::shared_ptr< GroupDef > group( new GroupDef( name, materialName ) );
	_groups.push_back( group );
	_currentGroup = group.get();
}

void OBJLoader::processMaterialFile( std::string materialFileName )
{
	std::ifstream materialFile;
	materialFile.open( materialFileName );
	if ( !materialFile.is_open() ) {
		Log::Error << "Cannot find material file with path " << materialFileName << Log::End;
		return;
	}

	MaterialDef *currentMaterial = nullptr;

	while ( !materialFile.eof() ) {
		std::string what;
		materialFile >> what;

		if ( what == "newmtl" ) {
			std::string materialName;
			materialFile >> materialName;
			std::shared_ptr< MaterialDef > material( new MaterialDef( materialName ) );
			_materials[ material->name ] = material;
			currentMaterial = material.get();
		}
		else if ( what == "map_Kd" ) {
			std::string diffuseMapFileName;
			materialFile >> diffuseMapFileName;
			ImagePtr image( new ImageTGA( FileSystem::getInstance().extractDirectory( materialFileName ) + "/" + diffuseMapFileName ) );
			TexturePtr texture( new Texture( image ) );
			currentMaterial->diffuseMap = texture;
		}
		else if ( what.length() > 0 ) {
			// unknown object. discard the line
			char buffer[ 1024 ];
			materialFile.getline( buffer, 1024 );
			Log::Debug << "Skipping " << what << " " << buffer << Log::End;
		}
	}
}

NodePtr OBJLoader::generateScene( void )
{
	VertexFormat vf( _positionCount > 0 ? 3 : 0, 
					 0, // no color information is imported
					 _normalCount > 0 ? 3 : 0,
					 _textureCoordCount > 0 ? 2 : 0 );

	GroupPtr scene( new Group( _filePath ) );

	for ( auto group : _groups ) {
		if ( group->faces.size() == 0 ) {
			Log::Debug << "Skipping empty group " << group->name << Log::End;
			continue;
		}

		std::cout << "Processing group " << group->name << std::endl;

		std::vector< float > vertices;
		std::vector< unsigned short > indices;
		unsigned short idx = 0;
		for ( auto face : group->faces ) {
			std::vector< unsigned int > faceIndices = StringUtils::split< unsigned int >( face, '/' );
	
			if ( vf.hasPositions() ) {		
				unsigned int pIdx = faceIndices[ 0 ] - 1;
				vertices.push_back( _positions[ pIdx * 3 + 0 ] );
				vertices.push_back( _positions[ pIdx * 3 + 1 ] );
				vertices.push_back( _positions[ pIdx * 3 + 2 ] );
			}

			if ( vf.hasNormals() ) {
				unsigned int nIdx = faceIndices[ 2 ] - 1;
				vertices.push_back( _normals[ nIdx * 3 + 0 ] );
				vertices.push_back( _normals[ nIdx * 3 + 1 ] );
				vertices.push_back( _normals[ nIdx * 3 + 2 ] );
			}

			if ( vf.hasTextureCoords() ) {
				unsigned int uvIdx = faceIndices[ 1 ] - 1;
				vertices.push_back( _textureCoords[ uvIdx * 2 + 0 ] );
				vertices.push_back( 1.0f - _textureCoords[ uvIdx * 2 + 1 ] );
			}

			indices.push_back( idx++ );
		}

		unsigned int vertexCount = vertices.size() / vf.getVertexSize();

		VertexBufferObjectPtr vbo( new VertexBufferObject( vf, vertexCount, &vertices[ 0 ] ) );
		IndexBufferObjectPtr ibo( new IndexBufferObject( indices.size(), &indices[ 0 ] ) );
	
		PrimitivePtr primitive( new Primitive( Primitive::Type::TRIANGLES ) );
		primitive->setVertexBuffer( vbo );
		primitive->setIndexBuffer( ibo );

		GeometryPtr geometry( new Geometry() );
		geometry->attachPrimitive( primitive );

		auto materialDef = _materials[ group->materialName ];
		if ( materialDef != nullptr ) {
			MaterialPtr material( new Material() );
			material->setColorMap( materialDef->diffuseMap );
			geometry->getComponent< MaterialComponent >()->attachMaterial( material );
		}

		scene->attachNode( geometry );
	}

	return scene;
}

int main( int argc, char **argv )
{
	SimulationPtr sim( new GLSimulation( "IronMan", argc, argv ) );

	GroupPtr scene( new Group() );

	OBJLoader loader( "ironman/Iron_Man.obj" );
	NodePtr ironman = loader.load();
	if ( ironman != nullptr ) {
		RotationComponentPtr rotationComponent( new RotationComponent( Vector3f( 0, 1, 0 ), 0.1 ) );
		ironman->attachComponent( rotationComponent );
		scene->attachNode( ironman );
	}

	CameraPtr camera( new Camera() );
	camera->local().setTranslate( 0.0f, 1.5f, 10.0f );
	scene->attachNode( camera );

	sim->attachScene( scene );
	return sim->run();
}

