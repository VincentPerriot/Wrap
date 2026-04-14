#pragma once
#include "vulkan/vulkan_core.h"

class VulkanCommands
{
public:
	static VkCommandBuffer beginSingleTimeCommands( VkDevice _device, VkCommandPool _pool );
	static void endSingleTimeCommands( VkCommandBuffer _commandBuffer, VkDevice _device, VkCommandPool _pool, VkQueue _queue );
};

