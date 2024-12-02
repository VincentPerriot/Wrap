#include "TriangleApp.h"

constexpr u32 WINDOW_WIDTH = 1920;
constexpr u32 WINDOW_HEIGHT = 1080;

//--------------------------------------------------------------------
TriangleApp::TriangleApp()
{
	Display::Instance( WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan" );
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
	}
}
