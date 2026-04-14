#pragma once

#include "../Utils/Common.h"
#include "vulkan/vulkan_core.h"

namespace Engine
{
	class Image
	{
	public:
		static void createTextureImage( VkDevice _device, VkPhysicalDevice _physDevice, std::string_view _path, std::optional<VkSampleCountFlagBits> _oMSAASamples,
			VkImage& _image, VkDeviceMemory& _imageMemory );

	private:
		static VkSampleCountFlagBits getMaxSamples( VkPhysicalDevice _physDevice );
		void transitionImageLayout( VkImage _image, VkFormat _format, VkImageLayout _old, VkImageLayout _new );
	};

} // end namespace Engine
