#include "TriangleApp.h"

//--------------------------------------------------------------------
TriangleApp::TriangleApp()
	: m_pRenderer( std::make_unique<Renderer>() )
	, m_pDisplay( std::make_unique<Display>( 800, 600, "Vulkan" ) )
{
}

TriangleApp::~TriangleApp()
{

}

//--------------------------------------------------------------------
void TriangleApp::run()
{
	initVulkan();

	mainLoop();

	cleanUp();
}

//--------------------------------------------------------------------
void TriangleApp::initVulkan()
{
	assert( m_pDisplay != nullptr );
	assert( m_pRenderer != nullptr );
}

//--------------------------------------------------------------------
void TriangleApp::mainLoop()
{
	while ( !m_pDisplay->shouldClose() )
	{
		m_pDisplay->pollEvents();
	}
}

//--------------------------------------------------------------------
void TriangleApp::cleanUp()
{
	m_pDisplay->cleanUp();
}
