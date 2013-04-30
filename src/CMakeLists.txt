INCLUDE( MacroAddSources )
INCLUDE( MacroExtractArchive )
INCLUDE( ExternalProject )

SET( CRIMILD_LIBRARY_NAME crimild-gl )

IF ( NOT EXISTS ${CRIMILD_GL_SOURCE_DIR}/third-party/glfw-2.7.8 )
	EXTRACT_ARCHIVE( glfw-2.7.8.zip glfw-2.7.8 ${CRIMILD_GL_SOURCE_DIR}/third-party )

	IF ( APPLE )
		MESSAGE( "-- Building GLFW v2.7.8 for Cocoa" )
		EXECUTE_PROCESS( COMMAND make -s cocoa WORKING_DIRECTORY ${CRIMILD_GL_SOURCE_DIR}/third-party/glfw-2.7.8 )
		EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E copy 
			${CRIMILD_GL_SOURCE_DIR}/third-party/glfw-2.7.8/lib/cocoa/libglfw.a
			${CRIMILD_GL_SOURCE_DIR}/lib/libglfw.a )
	ENDIF ( APPLE )
ENDIF ( NOT EXISTS ${CRIMILD_GL_SOURCE_DIR}/third-party/glfw-2.7.8 )

IF ( NOT EXISTS ${CRIMILD_GL_SOURCE_DIR}/third-party/glew-1.9.0 )
	EXTRACT_ARCHIVE( glew-1.9.0.tgz glew-1.9.0 ${CRIMILD_GL_SOURCE_DIR}/third-party )
ENDIF ( NOT EXISTS ${CRIMILD_GL_SOURCE_DIR}/third-party/glew-1.9.0 )

SET( CRIMILD_INCLUDE_DIRECTORIES 
	${CRIMILD_SOURCE_DIR}/src 
	${CRIMILD_GL_SOURCE_DIR}/src 
	${CRIMILD_GL_SOURCE_DIR}/third-party/glfw-2.7.8/include
	${CRIMILD_GL_SOURCE_DIR}/third-party/glew-1.9.0/include )

SET( CRIMILD_LINK_DIRECTORIES 
	${CRIMILD_SOURCE_DIR}/lib
	${CRIMILD_GL_SOURCE_DIR}/lib )

SET( CRIMILD_LIBRARY_LINK_LIBRARIES 
	crimild
	glfw )

ADD_SOURCES( ${CRIMILD_GL_SOURCE_DIR}/src *.hpp )
ADD_SOURCES( ${CRIMILD_GL_SOURCE_DIR}/src *.cpp )

IF ( APPLE )
	ADD_SOURCES( ${CRIMILD_GL_SOURCE_DIR}/third-party/glew-1.9.0/src glew.c )
ENDIF ( APPLE )

INCLUDE( ModuleBuildLibrary )
