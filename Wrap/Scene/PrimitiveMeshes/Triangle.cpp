#include "Triangle.h"


//--------------------------------------------------------------------
Scene::Primitives::Triangle::Triangle( std::string_view _name )
	: Mesh( _name )
{
	setVertices();
	setIndices();
}

//--------------------------------------------------------------------
Scene::Primitives::Triangle::~Triangle()
{

}

//--------------------------------------------------------------------
void Scene::Primitives::Triangle::setVertices()
{
	m_Vertices = {
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	};
}

//--------------------------------------------------------------------
void Scene::Primitives::Triangle::setIndices()
{
	m_Indices = { 0, 1, 2 };
}
