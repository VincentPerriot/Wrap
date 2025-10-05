#include "Scene.h"

#include <algorithm>


namespace Scene {
	//--------------------------------------------------------------------
	MeshHandle Scene::AddMesh( const Mesh& _mesh )
	{
		std::unique_lock<std::shared_mutex> lock( m_Mutex );

		m_MeshData.emplace_back( _mesh );

		auto handle{ MeshHandle{ m_NextMeshHandleId } };
		m_MeshHandles.emplace_back( handle );
		updateHandleToMeshIdx();

		m_NextMeshHandleId++;

		return handle;
	}

	//--------------------------------------------------------------------
	void Scene::updateHandleToMeshIdx()
	{
		m_HandleToMeshIdxMap.clear();

		size_t meshIdx = 0;
		for ( auto handle : m_MeshHandles )
		{
			if ( handle.isValid() )
			{
				m_HandleToMeshIdxMap.emplace_back( std::pair{ handle, meshIdx } );
				meshIdx++;
			}
		}
	}

	//--------------------------------------------------------------------
	void Scene::translateMesh2D( MeshHandle _handle, const Maths::Vector2& _vector2 )
	{
		if ( GetMesh( _handle ) )
		{
			for ( auto& vert : GetMesh( _handle )->getVertices() )
			{
				vert.m_Pos = vert.m_Pos + _vector2;
			}
		}
	}

	//--------------------------------------------------------------------
	void Scene::RemoveMesh( MeshHandle _handle )
	{
		std::unique_lock<std::shared_mutex> lock( m_Mutex );
		auto handlePos = std::ranges::find( m_MeshHandles, _handle );
		auto pos = std::ranges::find_if( m_HandleToMeshIdxMap, [&]( const auto& _pairing ) { return _pairing.first == _handle; } );

		bool process = pos != m_HandleToMeshIdxMap.end() && handlePos != m_MeshHandles.end();
		assert( process );

		if ( pos != m_HandleToMeshIdxMap.end() )
		{
			auto idx = pos->second;

			process = process && idx < m_MeshData.size();
			assert( process );

			if ( process )
			{
				if ( idx < ( m_MeshData.size() - 1 ) )
				{
					std::swap( m_MeshData.at( idx ), m_MeshData.back() );
				}

				m_MeshData.pop_back();
				m_HandleToMeshIdxMap.back().second = idx;

				handlePos->invalidate();

				updateHandleToMeshIdx();
			}
		}
	}

	//--------------------------------------------------------------------
	Mesh* Scene::GetMesh( std::string_view _name )
	{
		std::shared_lock<std::shared_mutex> lock( m_Mutex );
		auto pos = std::ranges::find_if( m_MeshData, [&]( const auto& _mesh ) { return _mesh.getName() == _name; } );

		if ( pos != m_MeshData.end() )
		{
			return &*pos;
		}
		else
		{
			assert( false );
			return nullptr;
		}
	}

	//--------------------------------------------------------------------
	Mesh* Scene::GetMesh( MeshHandle _handle )
	{
		std::shared_lock<std::shared_mutex> lock( m_Mutex );
		auto pos = std::ranges::find_if( m_HandleToMeshIdxMap, [&]( const auto& _pairing ) { return _pairing.first == _handle; } );
		assert( pos != m_HandleToMeshIdxMap.end() );

		if ( pos != m_HandleToMeshIdxMap.end() )
		{
			auto idx = pos->second;
			auto process = idx < m_MeshData.size();
			assert( process );

			if ( process )
			{
				return &m_MeshData.at( idx );
			}
		}

		assert( false );
		return nullptr;
	}

	//--------------------------------------------------------------------
	void Scene::SendToRender( Engine::Renderer& _renderer )
	{
		std::shared_lock<std::shared_mutex> lock( m_Mutex );
		_renderer.loadMeshes( m_MeshData );
	}

} // end namespace Scene