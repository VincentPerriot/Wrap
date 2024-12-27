#include "TriangleApp.h"

//--------------------------------------------------------------------
TriangleApp::TriangleApp()
{
	Display::Instance( Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT, "Vulkan" );
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
	m_pRenderer->init( Display::Instance().getWindowPtr() );
}

//--------------------------------------------------------------------
void TriangleApp::mainLoop()
{
	while ( !Display::Instance().shouldClose() )
	{
		Display::Instance().pollEvents();
		m_pRenderer->drawFrames();
	}
}
