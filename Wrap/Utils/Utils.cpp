#include "Utils.h"

//------------------------------------------------------------------------------------
u32 Utils::getMeshesVerticesCount( const std::vector<Scene::Mesh>& _meshes )
{
	u32 res{};
	for ( const auto& mesh : _meshes )
	{
		auto num = mesh.getVertices().size();

		res += (u32)( num );
	}

	return res;
}

//------------------------------------------------------------------------------------
u32 Utils::getMeshVerticesCount( const Scene::Mesh& _mesh )
{
	return _mesh.getVertices().size();
}
