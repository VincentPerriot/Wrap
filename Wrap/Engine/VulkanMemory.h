#pragma once

#include "vulkan/vulkan_core.h"
#include "../Scene/BaseScene.h"

namespace Engine::VulkanMemory {
	// Specific to Vertex Buffers
	void createMeshVertexBuffer( VkDevice _device, VkPhysicalDevice _physDevice, const Scene::Mesh& _mesh, VkBuffer& _buffer, VkDeviceMemory& _memory, VkCommandPool _pool, VkQueue _queue );
	void createMeshIndexBuffer( VkDevice _device, VkPhysicalDevice _physDevice, const Scene::Mesh& _mesh, VkBuffer& _buffer, VkDeviceMemory& _memory, VkCommandPool _pool, VkQueue _queue );

	// Generic
	void createBuffer( VkDevice _device, VkPhysicalDevice _physDevice, VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties, VkBuffer& _buffer, VkDeviceMemory& _memory );
	u32 findMemoryType( VkPhysicalDevice _physicalDevice, u32 _typeFilter, VkMemoryPropertyFlags _props );
	void copyBuffer( VkDevice _device, VkBuffer _source, VkBuffer _dest, VkDeviceSize _size, VkCommandPool _pool, VkQueue _queue );
} // end namespace Engine
