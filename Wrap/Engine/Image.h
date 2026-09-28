#pragma once

#include "../Utils/Common.h"
#include "vulkan/vulkan_core.h"

namespace Engine::VulkanImage
{
	void createTextureImage( VkDevice _device, VkPhysicalDevice _physDevice, VkCommandPool _pool, VkQueue _queue, std::string_view _path, std::optional<VkSampleCountFlagBits> _oMSAASamples,
		VkImage& _image, VkDeviceMemory& _imageMemory );
	VkSampleCountFlagBits getMaxSamples( VkPhysicalDevice _physDevice );
	void transitionImageLayout( VkDevice _device, VkCommandPool _pool, VkQueue _queue, VkImage _image, VkFormat _format, VkImageLayout _old, VkImageLayout _new );
	void copyBufferToImage( VkDevice _device, VkCommandPool _pool, VkBuffer _buffer, VkQueue _queue, VkImage _image, u32 _width, u32 _height );
} // end namespace Engine::Image
