#include "TriangleApp.h"

//--------------------------------------------------------------------
TriangleApp::TriangleApp()
	: m_pDisplay( std::make_unique<Display>( 1920, 1080, "Vulkan" ) )
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
}

//--------------------------------------------------------------------
void TriangleApp::initVulkan()
{
	m_pRenderer = std::make_unique<Engine::Renderer>();
	m_pRenderer->init( m_pDisplay->getWindowPtr() );
}

//--------------------------------------------------------------------
void TriangleApp::mainLoop()
{
	while ( !m_pDisplay->shouldClose() )
	{
		m_pDisplay->pollEvents();
	}
}
