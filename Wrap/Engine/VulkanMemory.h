#pragma once

#include "vulkan/vulkan_core.h"
#include "../Scene/Scene.h"

namespace Engine {

	class VulkanMemory
	{
	public:
		// Specific to Vertex Buffers
		static void createEmptyMeshVertexBuffer( VkDevice _device, const Scene::Mesh& _mesh, VkBuffer& _vertexBuffer );
		static void fillMeshVertexBuffer( VkDevice _device, const Scene::Mesh& _mesh, VkBuffer& _vertexBuffer, VkDeviceMemory& _deviceMem );

		// Generic
		static void createBufferMemory( VkPhysicalDevice _physicalDevice, VkDevice _device, VkBuffer& _buffer, VkDeviceMemory& _deviceMem );
		static u32 findMemoryType( VkPhysicalDevice _physicalDevice, u32 _typeFilter, VkMemoryPropertyFlags _props );
	};

} // end namespace Engine
