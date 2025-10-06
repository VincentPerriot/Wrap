#include "Quad.h"

//--------------------------------------------------------------------
Scene::Primitives::Quad::Quad( std::string_view _name )
	: Mesh( _name )
{
	setVertices();
	setIndices();
}

//--------------------------------------------------------------------
Scene::Primitives::Quad::~Quad()
{

}

//--------------------------------------------------------------------
void Scene::Primitives::Quad::setVertices()
{
	m_Vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};
}

//--------------------------------------------------------------------
void Scene::Primitives::Quad::setIndices()
{
	m_Indices = { 0, 1, 2, 2, 3, 0 };
}
