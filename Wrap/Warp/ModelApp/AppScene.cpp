#include "AppScene.h"


//--------------------------------------------------------------------------------
App::ModelApp::AppScene::AppScene( Engine::Renderer& _renderer )
	: m_Renderer( _renderer )
{
	start();
}

//--------------------------------------------------------------------------------
App::ModelApp::AppScene::~AppScene()
{

}

//--------------------------------------------------------------------------------
void App::ModelApp::AppScene::addGeometry()
{
	m_Triangle1 = AddMesh( ::Scene::Primitives::Triangle( "Tri_1" ) );
	m_Triangle2 = AddMesh( ::Scene::Primitives::Triangle( "Tri_2" ) );

	Maths::Vector2 vec1{ 0.4f, 0.4f };
	if ( m_Triangle1.isValid() )
		translateMesh2D( m_Triangle1, vec1 );

	Maths::Vector2 vec2{ -0.4f, -0.4f };
	if ( m_Triangle2.isValid() )
		translateMesh2D( m_Triangle2, vec2 );

	SendToRender( m_Renderer );
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
	//auto  now = std::chrono::steady_clock::now();
	//auto delta = std::chrono::duration_cast<std::chrono::milliseconds>( now - m_LastTimeUpdate );
	//m_LastTimeUpdate = now;
	//m_SceneTime += ( delta.count() / 1000.0 );

	//if ( m_SceneTime > 20.0f )
	//	RemoveMesh( m_Triangle2 );

}
