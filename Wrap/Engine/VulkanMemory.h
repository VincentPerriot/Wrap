#pragma once

#include "vulkan/vulkan_core.h"
#include "../Scene/Scene.h"

namespace Engine {

	class VulkanMemory
	{
	public:
		// Specific to Vertex Buffers
		static void createMeshVertexBuffer( VkDevice _device, VkPhysicalDevice _physDevice, const Scene::Mesh& _mesh, VkBuffer& _buffer, VkDeviceMemory& _memory, VkCommandPool _pool, VkQueue _queue );

		// Generic
		static void createBuffer( VkDevice _device, VkPhysicalDevice _physDevice, VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties, VkBuffer& _buffer, VkDeviceMemory& _memory );
		static u32 findMemoryType( VkPhysicalDevice _physicalDevice, u32 _typeFilter, VkMemoryPropertyFlags _props );
		static void copyBuffer( VkDevice _device, VkBuffer _source, VkBuffer _dest, VkDeviceSize _size, VkCommandPool _pool, VkQueue _queue );
	};

} // end namespace Engine
