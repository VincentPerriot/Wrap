#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "VulkanMemory.h"
#include "VulkanCommands.h"

namespace Engine::VulkanImage
{
	//----------------------------------------------------------------------------------
	void createTextureImage( VkDevice _device, VkPhysicalDevice _physDevice, VkCommandPool _pool, VkQueue _queue, std::string_view _path, std::optional<VkSampleCountFlagBits> _oMSAASamples,
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

		transitionImageLayout( _device, _pool, _queue, _image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );
		copyBufferToImage( _device, _pool, stagingBuffer, _queue, _image, static_cast<u32>( texWidth ), static_cast<u32>( texHeight ) );
		transitionImageLayout( _device, _pool, _queue, _image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );

		vkDestroyBuffer( _device, stagingBuffer, nullptr );
		vkFreeMemory( _device, stagingBufferMemory, nullptr );
	}

	//----------------------------------------------------------------------------------
	VkSampleCountFlagBits getMaxSamples( VkPhysicalDevice _physDevice )
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
	void transitionImageLayout( VkDevice _device, VkCommandPool _pool, VkQueue _queue, VkImage _image, VkFormat _format, VkImageLayout _old, VkImageLayout _new )
	{
		VkCommandBuffer cmdBuffer = VulkanCommands::beginSingleTimeCommands( _device, _pool );

		VkImageMemoryBarrier barrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = nullptr,
			.srcAccessMask = 0,
			.dstAccessMask = 0,
			.oldLayout = _old,
			.newLayout = _new,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = _image,
			.subresourceRange = VkImageSubresourceRange {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if ( _old == VK_IMAGE_LAYOUT_UNDEFINED && _new == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL )
		{
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if ( _old == VK_IMAGE_LAYOUT_UNDEFINED && _new == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL )
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
		{
			assert( false && "Layout transition not supported" );
		}

		vkCmdPipelineBarrier( cmdBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier );

		VulkanCommands::endSingleTimeCommands( cmdBuffer, _device, _pool, _queue );
	}

	//----------------------------------------------------------------------------------
	void copyBufferToImage( VkDevice _device, VkCommandPool _pool, VkBuffer _buffer, VkQueue _queue, VkImage _image, u32 _width, u32 _height )
	{
		VkCommandBuffer cmdBuffer = VulkanCommands::beginSingleTimeCommands( _device, _pool );

		VkBufferImageCopy region{
			.bufferOffset = 0,
			.bufferRowLength = 0,
			.bufferImageHeight = 0,
			.imageSubresource = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1
			},
			.imageOffset = {0, 0, 0},
			.imageExtent = {
				.width = _width,
				.height = _height,
				.depth = 1
			}
		};

		vkCmdCopyBufferToImage( cmdBuffer, _buffer, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region );

		VulkanCommands::endSingleTimeCommands( cmdBuffer, _device, _pool, _queue );
	}

} // end namespace Engine
