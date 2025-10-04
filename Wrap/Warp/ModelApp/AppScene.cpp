#include "AppScene.h"


//--------------------------------------------------------------------------------
App::ModelApp::AppScene::AppScene()
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
}

//--------------------------------------------------------------------------------
void App::ModelApp::AppScene::start()
{
	addGeometry();
}

//--------------------------------------------------------------------------------
void App::ModelApp::AppScene::update( Engine::Renderer& _renderer )
{
	Maths::Vector2 vec{ 0.1, 0.1 };
	translateMesh2D( m_Triangle1, vec );

	SendToRender( _renderer );
}
