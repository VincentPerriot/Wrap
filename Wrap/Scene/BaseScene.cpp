#include "BaseScene.h"

#include <algorithm>


namespace Scene {
	//--------------------------------------------------------------------
	MeshHandle BaseScene::addMesh( const Mesh& _mesh )
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
	void BaseScene::updateHandleToMeshIdx()
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
	void BaseScene::setProjection( f32 _fov, f32 _aspect, f32 _near, f32 _far, Maths::AngleUnit _angleUnit /*= Maths::AngleUnit::DEGREES*/ )
	{

	}

	//--------------------------------------------------------------------
	void BaseScene::updateCamera( Engine::Renderer& _renderer, const Camera& _cam )
	{

	}

	//--------------------------------------------------------------------
	void BaseScene::updateMeshPosition( MeshHandle _handle, Maths::Vector3 _position )
	{

	}

	//--------------------------------------------------------------------
	void BaseScene::updateMeshScale( MeshHandle _handle, Maths::Vector3 _scale )
	{

	}

	//--------------------------------------------------------------------
	void BaseScene::updateMeshRotation( MeshHandle _handle, Maths::Vector3 _rotation )
	{

	}

	//--------------------------------------------------------------------
	void BaseScene::removeMesh( MeshHandle _handle, Engine::Renderer& _renderer )
	{
		std::unique_lock<std::shared_mutex> lock( m_Mutex );


		auto handleIt = std::ranges::find( m_MeshHandles, _handle );
		auto it = std::ranges::find_if( m_HandleToMeshIdxMap, [&]( const auto& _pairing ) { return _pairing.first == _handle; } );

		bool process = it != m_HandleToMeshIdxMap.end() && handleIt != m_MeshHandles.end();
		assert( process );

		if ( it != m_HandleToMeshIdxMap.end() )
		{
			auto idx = it->second;
			process = process && idx < m_MeshData.size();
			assert( process );
			if ( process )
			{
				_renderer.removeMesh( m_MeshData[idx] );

				if ( idx < ( m_MeshData.size() - 1 ) )
				{
					std::swap( m_MeshData.at( idx ), m_MeshData.back() );
				}

				m_MeshData.pop_back();
				m_HandleToMeshIdxMap.back().second = idx;
				handleIt->invalidate();
				updateHandleToMeshIdx();

			}
		}
	}

	//--------------------------------------------------------------------
	Mesh* BaseScene::getMesh( std::string_view _name )
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
	Mesh* BaseScene::getMesh( MeshHandle _handle )
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
	void BaseScene::rendererUpdateMeshes( Engine::Renderer& _renderer )
	{
		std::shared_lock<std::shared_mutex> lock( m_Mutex );
		_renderer.loadMeshes( m_MeshData );
	}

} // end namespace Scene