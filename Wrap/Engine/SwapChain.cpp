#include "SwapChain.h"
#include "../Platforms/Windows/Display.h"

//------------------------------------------------------------------------------------
Engine::SwapChain::SwapChain( VkPhysicalDevice& _physicalDevice, VkDevice& _device, VkSurfaceKHR& _surface )
	: m_Device( _device )
{
	querySwapChainDetails( _physicalDevice, _surface );
	createSwapChain( _surface );
}


//------------------------------------------------------------------------------------
Engine::SwapChain::~SwapChain()
{
	vkDestroySwapchainKHR( m_Device, m_VkSwapChain, nullptr );
	for ( auto& imageview : m_ImageViews )
	{
		vkDestroyImageView( m_Device, imageview, nullptr );
	}
}

//------------------------------------------------------------------------------------
bool Engine::SwapChain::isAdequate()
{
	return !m_SupportDetails.m_PresentModes.empty() && !m_SupportDetails.m_Formats.empty();
}

//------------------------------------------------------------------------------------
void Engine::SwapChain::querySwapChainDetails( VkPhysicalDevice& _physicalDevice, VkSurfaceKHR& _surface )
{
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR( _physicalDevice, _surface, &m_SupportDetails.m_Capabilities );

	u32 formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR( _physicalDevice, _surface, &formatCount, nullptr );
	m_SupportDetails.m_Formats.resize( formatCount );
	vkGetPhysicalDeviceSurfaceFormatsKHR( _physicalDevice, _surface, &formatCount, m_SupportDetails.m_Formats.data() );

	u32 presentCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR( _physicalDevice, _surface, &presentCount, nullptr );
	m_SupportDetails.m_PresentModes.resize( presentCount );
	vkGetPhysicalDeviceSurfacePresentModesKHR( _physicalDevice, _surface, &presentCount, m_SupportDetails.m_PresentModes.data() );
}

//------------------------------------------------------------------------------------
VkSurfaceFormatKHR Engine::SwapChain::chooseSwapSurfaceFormat()
{
	for ( const auto& format : m_SupportDetails.m_Formats )
	{
		if ( format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR )
			return format;
	}
	return m_SupportDetails.m_Formats[0];
}

//------------------------------------------------------------------------------------
VkPresentModeKHR Engine::SwapChain::choosePresentMode()
{
	for ( const auto& mode : m_SupportDetails.m_PresentModes )
	{
		if ( mode == VK_PRESENT_MODE_MAILBOX_KHR )
			return mode;
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

//------------------------------------------------------------------------------------
VkExtent2D Engine::SwapChain::chooseSwapExtent()
{
	if ( m_SupportDetails.m_Capabilities.currentExtent.width == std::numeric_limits<u32>::max() )
		return m_SupportDetails.m_Capabilities.currentExtent;
	else {
		int width, height;
		glfwGetFramebufferSize( Display::Instance().getWindowPtr(), &width, &height );

		VkExtent2D extent = { (u32)width, (u32)height };
		extent.width = std::clamp( extent.width, m_SupportDetails.m_Capabilities.minImageExtent.width, m_SupportDetails.m_Capabilities.maxImageExtent.width );
		extent.height = std::clamp( extent.height, m_SupportDetails.m_Capabilities.minImageExtent.height, m_SupportDetails.m_Capabilities.maxImageExtent.height );

		return extent;
	}
}

//------------------------------------------------------------------------------------
void Engine::SwapChain::createSwapChain( VkSurfaceKHR& _surface )
{
	m_SelectedFormat = chooseSwapSurfaceFormat();
	m_SelectedPresentMode = choosePresentMode();
	VkExtent2D swapExtent = chooseSwapExtent();

	// Mailbox Requires Triple buffering
	u32 imageCount = m_SelectedPresentMode == VK_PRESENT_MODE_MAILBOX_KHR ? 3 : m_SupportDetails.m_Capabilities.minImageCount + 1;

	if ( m_SupportDetails.m_Capabilities.maxImageCount > 0 && imageCount > m_SupportDetails.m_Capabilities.maxImageCount )
	{
		imageCount = m_SupportDetails.m_Capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext = nullptr,
		.flags = 0,
		.surface = _surface,
		.minImageCount = imageCount,
		.imageFormat = m_SelectedFormat.format,
		.imageColorSpace = m_SelectedFormat.colorSpace,
		.imageExtent = swapExtent,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.preTransform = m_SupportDetails.m_Capabilities.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = m_SelectedPresentMode,
		.clipped = VK_TRUE,
		.oldSwapchain = VK_NULL_HANDLE
	};

	VK_ASSERT( vkCreateSwapchainKHR( m_Device, &createInfo, nullptr, &m_VkSwapChain ) );

	vkGetSwapchainImagesKHR( m_Device, m_VkSwapChain, &imageCount, nullptr );
	m_SwapChainImages.resize( imageCount );
	vkGetSwapchainImagesKHR( m_Device, m_VkSwapChain, &imageCount, m_SwapChainImages.data() );

}

//------------------------------------------------------------------------------------
void Engine::SwapChain::createImageViews()
{
	m_ImageViews.resize( m_SwapChainImages.size() );

	for ( size_t i = 0; i < m_SwapChainImages.size(); i++ )
	{
		VkComponentMapping components{
			.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.a = VK_COMPONENT_SWIZZLE_IDENTITY
		};

		VkImageSubresourceRange subResources{
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		};

		VkImageViewCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.image = m_SwapChainImages[i],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = m_SelectedFormat.format,
			.components = components,
			.subresourceRange = subResources
		};

		VK_ASSERT( vkCreateImageView( m_Device, &createInfo, nullptr, &m_ImageViews[i] ) );
	}
}

