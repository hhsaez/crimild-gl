#include <Crimild.hpp>
#include <CrimildGL.hpp>

int main( int argc, char **argv )
{
	Crimild::SimulationPtr sim( new Crimild::GLSimulation( "test" ) );
	return sim->run();
}

