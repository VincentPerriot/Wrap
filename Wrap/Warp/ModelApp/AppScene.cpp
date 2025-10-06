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
	m_Triangle3 = AddMesh( ::Scene::Primitives::Triangle( "Tri_3" ) );
	m_Triangle4 = AddMesh( ::Scene::Primitives::Triangle( "Tri_4" ) );

	Maths::Vector2 vec1{ 0.4f, 0.4f };
	if ( m_Triangle1.isValid() )
		translateMesh2D( m_Triangle1, vec1 );

	Maths::Vector2 vec2{ -0.4f, -0.4f };
	if ( m_Triangle2.isValid() )
		translateMesh2D( m_Triangle2, vec2 );

	Maths::Vector2 vec3{ -0.4f, 0.4f };
	if ( m_Triangle3.isValid() )
		translateMesh2D( m_Triangle3, vec3 );

	Maths::Vector2 vec4{ 0.4f, -0.4f };
	if ( m_Triangle4.isValid() )
		translateMesh2D( m_Triangle4, vec4 );

	RendererUpdateMeshes( m_Renderer );
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

	if ( m_SceneTime > 5.0f && m_RemoveTest1 )
	{
		RemoveMesh( m_Triangle2, m_Renderer );
		m_RemoveTest1 = false;
	}

	if ( m_SceneTime > 10.0f && m_RemoveTest2 )
	{
		RemoveMesh( m_Triangle1, m_Renderer );
		m_RemoveTest2 = false;
	}

	if ( m_SceneTime > 15.0f && m_RemoveTest3 )
	{
		RemoveMesh( m_Triangle4, m_Renderer );
		m_RemoveTest3 = false;
	}
}
