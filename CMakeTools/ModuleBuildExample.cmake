# Build an example application, linking it with Crimild libraries
# Arguments:
# CRIMILD_EXAMPLE_NAME: (Required) Name for the example project

MESSAGE( "   " ${CRIMILD_EXAMPLE_NAME} )

FILE( GLOB_RECURSE CRIMILD_EXAMPLE_HEADER_FILES "${CRIMILD_GL_SOURCE_DIR}/examples/${CRIMILD_EXAMPLE_NAME}/*.hpp" )
FILE( GLOB_RECURSE CRIMILD_EXAMPLE_SOURCE_FILES "${CRIMILD_GL_SOURCE_DIR}/examples/${CRIMILD_EXAMPLE_NAME}/*.cpp" )

SET( CRIMILD_EXAMPLE_DEPENDENCIES 
	crimild
	crimild-gl )

SET( CRIMILD_EXAMPLES_LINK_LIBRARIES 
	crimild
	crimild-gl )

INCLUDE_DIRECTORIES(
	${CRIMILD_SOURCE_DIR}/src 
	${CRIMILD_GL_SOURCE_DIR}/src )

LINK_DIRECTORIES(
	${CRIMILD_SOURCE_DIR}/lib
	${CRIMILD_GL_SOURCE_DIR}/lib )

IF ( APPLE )
	SET( CRIMILD_EXAMPLES_LINK_LIBRARIES 
		${CRIMILD_EXAMPLES_LINK_LIBRARIES} 
		"-framework Cocoa -framework OpenGL -framework IOKit" )
ENDIF ( APPLE )

ADD_EXECUTABLE( ${CRIMILD_EXAMPLE_NAME}
	${CRIMILD_EXAMPLE_SOURCE_FILES}
	${CRIMILD_EXAMPLE_HEADER_FILES} )
TARGET_LINK_LIBRARIES( ${CRIMILD_EXAMPLE_NAME} ${CRIMILD_EXAMPLES_LINK_LIBRARIES} )
ADD_DEPENDENCIES( ${CRIMILD_EXAMPLE_NAME} ${CRIMILD_EXAMPLE_DEPENDENCIES} )

