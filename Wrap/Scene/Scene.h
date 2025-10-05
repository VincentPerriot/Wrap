#pragma once

#include "../Utils/Common.h"
#include <unordered_map>
#include <shared_mutex>

#include "Mesh.h"
#include "../Engine/Renderer.h"

namespace Scene {

	struct MeshHandle {
		u32 m_Id;
		bool m_Valid = { true };

		bool isValid() { return m_Valid; };
		void invalidate() { m_Valid = false; };

		constexpr bool operator==( const MeshHandle& _other ) const
		{
			return m_Id == _other.m_Id && m_Valid == _other.m_Valid;
		}
	};

	struct Camera {
		Maths::Vector3 m_Pos;
	};

	class Scene
	{
	public:
		MeshHandle AddMesh( const Mesh& _mesh );
		void updateHandleToMeshIdx();

		void translateMesh2D( MeshHandle _handle, const Maths::Vector2& _vector2 );

		void RemoveMesh( MeshHandle _handle, Engine::Renderer& _renderer );
		Mesh* GetMesh( MeshHandle _handle );
		Mesh* GetMesh( std::string_view _name );

		void RendererUpdateMeshes( Engine::Renderer& _renderer );

	private:
		u32 m_NextMeshHandleId{ 0 };
		std::vector<std::pair<MeshHandle, size_t>> m_HandleToMeshIdxMap;
		std::vector<MeshHandle> m_MeshHandles;
		std::vector<Mesh> m_MeshData;
		Camera m_Camera;

		mutable std::shared_mutex m_Mutex;
	};

} // end namespace Scene
