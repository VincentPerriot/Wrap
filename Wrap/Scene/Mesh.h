#pragma once

#include "../Utils/Common.h"
#include "../Engine/VulkanTypes.h"

namespace Scene {
	class Mesh
	{
	private:
		std::vector<Engine::Vertex> m_Vertices;
	};
} // end namespace Scene
