#pragma once

#include "../Utils/Common.h"
#include <unordered_map>
#include <shared_mutex>

#include "Mesh.h"
#include "../Engine/Renderer.h"
#include "../Maths/Vector3.h"

namespace Scene {

	struct MeshHandle 
	{
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
		Maths::Vector3 m_EyePos;
		Maths::Vector3 m_LookAt;
		Maths::Vector3 m_WorldUp;
	};

	class BaseScene
	{
	public:
		MeshHandle addMesh( const Mesh& _mesh );
		void updateHandleToMeshIdx();

		void setProjection( f32 _fov, f32 _aspect, f32 _near, f32 _far, Maths::AngleUnit _angleUnit = Maths::AngleUnit::DEGREES );

		void updateCamera( Engine::Renderer& _renderer, const Camera& _cam );

		void updateMeshPosition( MeshHandle _handle, Maths::Vector3 _position );
		void updateMeshScale( MeshHandle _handle, Maths::Vector3 _scale );
		void updateMeshRotation( MeshHandle _handle, Maths::Vector3 _rotation );

		void removeMesh( MeshHandle _handle, Engine::Renderer& _renderer );
		Mesh* getMesh( MeshHandle _handle );
		Mesh* getMesh( std::string_view _name );

		void rendererUpdateMeshes( Engine::Renderer& _renderer );

	private:
		void updateModelsTransform( Engine::Renderer& _renderer );

		u32 m_NextMeshHandleId{ 0 };
		std::vector<std::pair<MeshHandle, size_t>> m_HandleToMeshIdxMap;
		std::vector<MeshHandle> m_MeshHandles;
		std::vector<Mesh> m_MeshData;
		Camera m_Camera;

		mutable std::shared_mutex m_Mutex;
	};

} // end namespace Scene
