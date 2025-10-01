#include "ModelApp.h"

//--------------------------------------------------------------------
ModelApp::ModelApp()
{
	Display::Instance( Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT, "Vulkan" );
}

ModelApp::~ModelApp()
{

}

//--------------------------------------------------------------------
void ModelApp::run()
{
	initVulkan();

	mainLoop();
}

//--------------------------------------------------------------------
void ModelApp::initVulkan()
{
	m_pRenderer = std::make_unique<Engine::Renderer>();
	m_pRenderer->init( Display::Instance().getWindowPtr() );
}

//--------------------------------------------------------------------
void ModelApp::mainLoop()
{
	while ( !Display::Instance().shouldClose() )
	{
		Display::Instance().pollEvents();
		m_pRenderer->drawFrames();
	}
}