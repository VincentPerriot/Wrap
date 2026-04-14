#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "VulkanMemory.h"

namespace Engine
{
	//----------------------------------------------------------------------------------
	void Image::createTextureImage( VkDevice _device, VkPhysicalDevice _physDevice, std::string_view _path, std::optional<VkSampleCountFlagBits> _oMSAASamples,
		VkImage& _image, VkDeviceMemory& _imageMemory )
	{
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load( _path.data(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha );
		assert( pixels );

		VkDeviceSize imageSize = ( texWidth * texHeight ) * 4;
		u32 mipLevelsPossible = std::bit_width( std::max( (u32)texWidth, (u32)texHeight ) );
		u32 maxMipLevel = 8;
		u32 mipLevels = std::min( mipLevelsPossible, maxMipLevel );

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		VulkanMemory::createBuffer( _device, _physDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory );

		void* data;
		vkMapMemory( _device, stagingBufferMemory, 0, imageSize, 0, &data );
		memcpy( data, pixels, static_cast<size_t>( imageSize ) );
		vkUnmapMemory( _device, stagingBufferMemory );
		stbi_image_free( pixels );

		VkSampleCountFlagBits samples = getMaxSamples( _physDevice );
		if ( _oMSAASamples.has_value() && _oMSAASamples.value() < samples )
			samples = _oMSAASamples.value();

		VkImageCreateInfo imageInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = VK_FORMAT_R8G8B8_SRGB,
			.extent = VkExtent3D{.width = static_cast<u32>( texWidth ), .height = static_cast<u32>( texHeight ), .depth = 1 },
			.mipLevels = mipLevels,
			.arrayLayers = 1,
			.samples = samples,
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
		};

		VK_ASSERT( vkCreateImage( _device, &imageInfo, nullptr, &_image ) );

		VkMemoryRequirements memReq;
		vkGetImageMemoryRequirements( _device, _image, &memReq );

		VkMemoryAllocateInfo info{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = nullptr,
			.allocationSize = memReq.size,
			.memoryTypeIndex = VulkanMemory::findMemoryType( _physDevice, memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT )
		};

		VK_ASSERT( vkAllocateMemory( _device, &info, nullptr, &_imageMemory ) );

		vkBindImageMemory( _device, _image, _imageMemory, 0 );
	}

	//----------------------------------------------------------------------------------
	VkSampleCountFlagBits Image::getMaxSamples( VkPhysicalDevice _physDevice )
	{
		VkPhysicalDeviceProperties physDeviceProps;
		vkGetPhysicalDeviceProperties( _physDevice, &physDeviceProps );

		VkSampleCountFlags counts = physDeviceProps.limits.framebufferColorSampleCounts & physDeviceProps.limits.framebufferDepthSampleCounts;

		//if ( counts & VK_SAMPLE_COUNT_64_BIT ) { return VK_SAMPLE_COUNT_64_BIT; }
		//if ( counts & VK_SAMPLE_COUNT_32_BIT ) { return VK_SAMPLE_COUNT_32_BIT; }
		if ( counts & VK_SAMPLE_COUNT_16_BIT ) { return VK_SAMPLE_COUNT_16_BIT; }
		if ( counts & VK_SAMPLE_COUNT_8_BIT ) { return VK_SAMPLE_COUNT_8_BIT; }
		if ( counts & VK_SAMPLE_COUNT_4_BIT ) { return VK_SAMPLE_COUNT_4_BIT; }
		if ( counts & VK_SAMPLE_COUNT_2_BIT ) { return VK_SAMPLE_COUNT_2_BIT; }

		return VK_SAMPLE_COUNT_1_BIT;
	}

	//----------------------------------------------------------------------------------
	void Image::transitionImageLayout( VkImage _image, VkFormat _format, VkImageLayout _old, VkImageLayout _new )
	{

	}

} // end namespace Engine
