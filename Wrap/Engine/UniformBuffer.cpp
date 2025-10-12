#include "UniformBuffer.h"
#include "VulkanMemory.h"

//------------------------------------------------------------------------------------
Engine::UniformBuffer::UniformBuffer( VkDevice _device, VkPhysicalDevice _physDevice, VkDeviceSize _size )
	: m_Device( _device )
	, m_PhysDevice( _physDevice )
	, m_Size( _size )
{
	createBuffer();
}

//------------------------------------------------------------------------------------
Engine::UniformBuffer::~UniformBuffer()
{
	for ( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
	{
		vkDestroyBuffer( m_Device, m_Buffers[i], nullptr );
		vkFreeMemory( m_Device, m_MemoryArr[i], nullptr );
	}
}

//------------------------------------------------------------------------------------
void Engine::UniformBuffer::update( u32 _currentImage, const void* _data, size_t _size )
{
	memcpy( m_MappedMemoryArr[_currentImage], _data, _size );
}

//------------------------------------------------------------------------------------
void Engine::UniformBuffer::createBuffer()
{
	for ( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
	{
		VulkanMemory::createBuffer( m_Device, m_PhysDevice, m_Size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_Buffers[i], m_MemoryArr[i] );

		VK_ASSERT( vkMapMemory( m_Device, m_MemoryArr[i], 0, m_Size, 0, &m_MappedMemoryArr[i] ) );
	}
}
