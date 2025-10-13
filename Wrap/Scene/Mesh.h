#pragma once

#include "../Utils/Common.h"
#include <span>

#include "../Engine/VulkanTypes.h"
#include "../Maths/Matrix4.h"

namespace Scene {
	class Mesh
	{
	public:
		Mesh( std::string_view _name );
		virtual ~Mesh();

		bool operator==( const Mesh& _other ) const { return m_Name == _other.m_Name; };

		void setTransforms( const Maths::Matrix4& _mat );

		std::vector<Engine::Vertex>& getVertices() { return m_Vertices; };
		const std::vector<Engine::Vertex>& getVertices() const { return m_Vertices; };

		std::vector<u16>& getIndices() { return m_Indices; };
		const std::vector<u16>& getIndices() const { return m_Indices; };

		const Maths::Matrix4 getModelMat() const { return m_ModelMat; };
		constexpr std::string getName() const { return m_Name; };


	protected:
		virtual void setVertices();
		virtual void setIndices();

		std::string m_Name;
		std::vector<Engine::Vertex> m_Vertices{};
		std::vector<u16> m_Indices{};
		Maths::Matrix4 m_ModelMat = Maths::Matrix4::DefaultModelMatrix();
	};
} // end namespace Scene
