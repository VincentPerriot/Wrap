#include "AppScene.h"


//--------------------------------------------------------------------------------
App::ModelApp::AppScene::AppScene( Engine::Renderer& _renderer )
	: m_Renderer( _renderer )
{
	start();

	setInitialCamera( Scene::Camera {
		.m_EyePos = Maths::Vector3{ 0.0f, 2.0f, 2.0f },
		.m_LookAt = Maths::Vector3{ 0.0f, 0.0f, 0.0f },
		.m_WorldUp = Maths::Vector3{ 0.0f, 1.0f, 0.0f },
	} );
}

//--------------------------------------------------------------------------------
App::ModelApp::AppScene::~AppScene()
{

}

//--------------------------------------------------------------------------------
void App::ModelApp::AppScene::addGeometry()
{
	m_Quad1 = addMesh( ::Scene::Primitives::Quad( "Quad_1" ) );
	m_Quad2 = addMesh( ::Scene::Primitives::Quad( "Quad_2" ) );

	rendererUpdateMeshes( m_Renderer );
}

//--------------------------------------------------------------------------------
void App::ModelApp::AppScene::start()
{
	addGeometry();
	m_LastTimeUpdate = std::chrono::steady_clock::now();
}

//--------------------------------------------------------------------------------
void App::ModelApp::AppScene::update()
{
	auto now = std::chrono::steady_clock::now();
	std::chrono::duration<f32> delta = now - m_LastTimeUpdate;

	m_LastTimeUpdate = now;
	m_SceneTime += delta.count();


}
