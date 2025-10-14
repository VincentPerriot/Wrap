#include "Mesh.h"

//--------------------------------------------------------------------
Scene::Mesh::Mesh( std::string_view _name, const Maths::Matrix4& _modelMat )
	: m_Name( _name )
	, m_ModelMat( _modelMat )
{
}

//--------------------------------------------------------------------
Scene::Mesh::~Mesh()
{

}

//--------------------------------------------------------------------
void Scene::Mesh::setTransforms( const Maths::Matrix4& _mat )
{
	m_ModelMat = _mat;
}

//--------------------------------------------------------------------
void Scene::Mesh::setVertices()
{
}

//--------------------------------------------------------------------
void Scene::Mesh::setIndices()
{
}
