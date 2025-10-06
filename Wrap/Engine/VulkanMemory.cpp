#include "VulkanMemory.h"

namespace Engine {

	//------------------------------------------------------------------------------------
	void VulkanMemory::createBuffer( VkDevice _device, VkPhysicalDevice _physDevice, VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties, VkBuffer& _buffer, VkDeviceMemory& _memory )
	{
		VkBufferCreateInfo bufferInfo{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.size = _size,
			.usage = _usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr
		};

		VK_ASSERT( vkCreateBuffer( _device, &bufferInfo, nullptr, &_buffer ) );

		VkMemoryRequirements memReqs;
		vkGetBufferMemoryRequirements( _device, _buffer, &memReqs );

		VkMemoryAllocateInfo allocInfo{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = nullptr,
			.allocationSize = memReqs.size,
			.memoryTypeIndex = VulkanMemory::findMemoryType( _physDevice, memReqs.memoryTypeBits, _properties )
		};

		VK_ASSERT( vkAllocateMemory( _device, &allocInfo, nullptr, &_memory ) );

		VK_ASSERT( vkBindBufferMemory( _device, _buffer, _memory, 0 ) );
	}

	//------------------------------------------------------------------------------------
	void VulkanMemory::createMeshVertexBuffer( VkDevice _device, VkPhysicalDevice _physDevice, const Scene::Mesh& _mesh, VkBuffer& _buffer, VkDeviceMemory& _memory, VkCommandPool _pool, VkQueue _queue )
	{
		VkDeviceSize size = sizeof( Vertex ) * _mesh.getVertices().size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingMemory;
		createBuffer( _device, _physDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingMemory );

		void* data;
		VK_ASSERT( vkMapMemory( _device, stagingMemory, 0, size, 0, &data ) );
		memcpy( data, _mesh.getVertices().data(), (size_t)size );
		vkUnmapMemory( _device, stagingMemory );

		createBuffer( _device, _physDevice, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _buffer, _memory );

		copyBuffer( _device, stagingBuffer, _buffer, size, _pool, _queue );

		vkDestroyBuffer( _device, stagingBuffer, nullptr );
		vkFreeMemory( _device, stagingMemory, nullptr );
	}

	//------------------------------------------------------------------------------------
	void VulkanMemory::createMeshIndexBuffer( VkDevice _device, VkPhysicalDevice _physDevice, const Scene::Mesh& _mesh, VkBuffer& _buffer, VkDeviceMemory& _memory, VkCommandPool _pool, VkQueue _queue )
	{
		VkDeviceSize size = sizeof( u16 ) * _mesh.getIndices().size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingMemory;
		createBuffer( _device, _physDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingMemory );

		void* data;
		VK_ASSERT( vkMapMemory( _device, stagingMemory, 0, size, 0, &data ) );
		memcpy( data, _mesh.getIndices().data(), (size_t)size );
		vkUnmapMemory( _device, stagingMemory );

		createBuffer( _device, _physDevice, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _buffer, _memory );

		copyBuffer( _device, stagingBuffer, _buffer, size, _pool, _queue );

		vkDestroyBuffer( _device, stagingBuffer, nullptr );
		vkFreeMemory( _device, stagingMemory, nullptr );
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

		return 0;
	}

	//------------------------------------------------------------------------------------
	void VulkanMemory::copyBuffer( VkDevice _device, VkBuffer _source, VkBuffer _dest, VkDeviceSize _size, VkCommandPool _pool, VkQueue _queue )
	{
		VkCommandBufferAllocateInfo allocInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = nullptr,
			.commandPool = _pool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1
		};

		VkCommandBuffer commandBuffer;
		VK_ASSERT( vkAllocateCommandBuffers( _device, &allocInfo, &commandBuffer ) );

		VkCommandBufferBeginInfo beginInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = nullptr,
			.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
			.pInheritanceInfo = nullptr
		};

		VK_ASSERT( vkBeginCommandBuffer( commandBuffer, &beginInfo ) );

		VkBufferCopy copyRegion{
			.srcOffset = 0,
			.dstOffset = 0,
			.size = _size
		};

		vkCmdCopyBuffer( commandBuffer, _source, _dest, 1, &copyRegion );
		vkEndCommandBuffer( commandBuffer );

		VkSubmitInfo submitInfo{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pNext = nullptr,
			.waitSemaphoreCount = 0,
			.pWaitSemaphores = nullptr,
			.pWaitDstStageMask = nullptr,
			.commandBufferCount = 1,
			.pCommandBuffers = &commandBuffer,
			.signalSemaphoreCount = 0,
			.pSignalSemaphores = nullptr
		};

		VK_ASSERT( vkQueueSubmit( _queue, 1, &submitInfo, VK_NULL_HANDLE ) );
		vkQueueWaitIdle( _queue );

		vkFreeCommandBuffers( _device, _pool, 1, &commandBuffer );
	}

}
