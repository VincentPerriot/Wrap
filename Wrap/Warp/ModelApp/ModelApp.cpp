#include "ModelApp.h"

namespace App::ModelApp {
	//--------------------------------------------------------------------
	ModelApp::ModelApp()
	{
		Display::Instance( Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT, "Vulkan" );
	}

	//--------------------------------------------------------------------
	ModelApp::~ModelApp()
	{

	}

	//--------------------------------------------------------------------
	void ModelApp::run()
	{
		initVulkan();
		createScene();

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
			m_AppScene->update();
			m_pRenderer->drawFrames();
		}
	}

	//--------------------------------------------------------------------
	void ModelApp::createScene()
	{
		m_AppScene = std::make_unique<AppScene>( *m_pRenderer );
	}
} // end namespace App::ModelApp
