#include "VulkanCommands.h"
#include "Debug.h"

//------------------------------------------------------------------------------------
VkCommandBuffer VulkanCommands::beginSingleTimeCommands( VkDevice _device, VkCommandPool _pool )
{
	VkCommandBufferAllocateInfo cbaInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = _pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};

	VkCommandBuffer cmdBuffer;
	VK_ASSERT( vkAllocateCommandBuffers( _device, &cbaInfo, &cmdBuffer ) );

	VkCommandBufferBeginInfo cbInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
	};

	VK_ASSERT( vkBeginCommandBuffer( cmdBuffer, &cbInfo ) );

	return cmdBuffer;
}

//------------------------------------------------------------------------------------
void VulkanCommands::endSingleTimeCommands( VkCommandBuffer _commandBuffer, VkDevice _device, VkCommandPool _pool, VkQueue _queue )
{
	VkSubmitInfo subInfo{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = 0,
		.pWaitSemaphores = nullptr,
		.pWaitDstStageMask = nullptr,
		.commandBufferCount = 1,
		.pCommandBuffers = &_commandBuffer,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = nullptr
	};

	VkFenceCreateInfo vinfo{
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
	};
	VkFence fence;

	VK_ASSERT( vkCreateFence( _device, &vinfo, nullptr, &fence ) );
	VK_ASSERT( vkQueueSubmit( _queue, 1, &subInfo, VK_NULL_HANDLE ) );
	VK_ASSERT( vkWaitForFences( _device, 1, &fence, VK_TRUE, UINT64_MAX ) );

	vkDestroyFence( _device, fence, nullptr );
	vkFreeCommandBuffers( _device, _pool, 1, &_commandBuffer );
}
