#pragma once

#include "../Utils/Common.h"
#include <span>

#include "../Engine/VulkanTypes.h"

namespace Scene {
	class Mesh
	{
	public:
		Mesh( std::string_view _name );
		virtual ~Mesh();

		bool operator==( const Mesh& _other ) const { return m_Name == _other.m_Name; };

		std::vector<Engine::Vertex>& getVertices() { return m_Vertices; };
		const std::vector<Engine::Vertex>& getVertices() const { return m_Vertices; };

		std::vector<u32>& getIndices() { return m_Indices; };
		const std::vector<u32>& getIndices() const { return m_Indices; };

		constexpr std::string getName() const { return m_Name; };

	protected:
		virtual void setVertices();
		virtual void setIndices();

		std::string m_Name;
		std::vector<Engine::Vertex> m_Vertices{};
		std::vector<u32> m_Indices{};
	};
} // end namespace Scene
