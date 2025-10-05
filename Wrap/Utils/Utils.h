#pragma once

#include "Common.h"
#include "../Scene/Mesh.h"

class Utils
{
public:
	static u32 getMeshesVerticesCount( const std::vector<Scene::Mesh>& _meshes );
	static u32 getMeshVerticesCount( const Scene::Mesh& _mesh );
};

