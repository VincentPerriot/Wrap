#include "VulkanMemory.h"

namespace Engine {
	//------------------------------------------------------------------------------------
	void VulkanMemory::createEmptyMeshVertexBuffer( VkDevice _device, const Scene::Mesh& _mesh, VkBuffer& _vertexBuffer )
	{

		VkBufferCreateInfo bufferInfo{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.size = sizeof( Engine::Vertex ) * _mesh.getVertices().size(),
			.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr
		};

		VK_ASSERT( vkCreateBuffer( _device, &bufferInfo, nullptr, &_vertexBuffer ) );
	}

	//------------------------------------------------------------------------------------
	void VulkanMemory::createBufferMemory( VkPhysicalDevice _physicalDevice, VkDevice _device, VkBuffer& _buffer, VkDeviceMemory& _deviceMem )
	{
		VkMemoryRequirements memReq;
		vkGetBufferMemoryRequirements( _device, _buffer, &memReq );

		VkMemoryAllocateInfo allocInfo{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = nullptr,
			.allocationSize = memReq.size,
			.memoryTypeIndex = VulkanMemory::findMemoryType( _physicalDevice, memReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT )
		};

		VK_ASSERT( vkAllocateMemory( _device, &allocInfo, nullptr, &_deviceMem ) );

		VK_ASSERT( vkBindBufferMemory( _device, _buffer, _deviceMem, 0 ) );
	}

	//------------------------------------------------------------------------------------
	void VulkanMemory::fillMeshVertexBuffer( VkDevice _device, const Scene::Mesh& _mesh, VkBuffer& _vertexBuffer, VkDeviceMemory& _deviceMem )
	{
		void* data;
		size_t size = sizeof( Engine::Vertex ) * _mesh.getVertices().size();

		VK_ASSERT( vkMapMemory( _device, _deviceMem, 0, size, 0, &data ) );
		memcpy( data, _mesh.getVertices().data(), size );
		vkUnmapMemory( _device, _deviceMem );
	}

	//------------------------------------------------------------------------------------
	u32 VulkanMemory::findMemoryType( VkPhysicalDevice _physicalDevice, u32 _typeFilter, VkMemoryPropertyFlags _props )
	{
		VkPhysicalDeviceMemoryProperties memProps;
		vkGetPhysicalDeviceMemoryProperties( _physicalDevice, &memProps );

		// TODO -> Check out Heaps on Memory Properties
		for ( u32 i = 0; i < memProps.memoryTypeCount; i++ )
		{
			if ( ( _typeFilter & ( 1 << i ) )
				&& ( memProps.memoryTypes[i].propertyFlags & _props ) == _props )
			{
				return i;
			}
		}
	}

}
