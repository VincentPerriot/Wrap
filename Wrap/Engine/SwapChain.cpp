#include "SwapChain.h"
#include "../Platforms/Windows/Display.h"

//------------------------------------------------------------------------------------
Engine::SwapChain::SwapChain( VkPhysicalDevice& _physicalDevice, VkSurfaceKHR& _surface )
{
	querySwapChainDetails( _physicalDevice, _surface);
	createSwapChain();
}


//------------------------------------------------------------------------------------
Engine::SwapChain::~SwapChain()
{

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
		glfwGetFramebufferSize( Display::Instance().getWindowPtr(), &width, &height);

		VkExtent2D extent = { (u32)width, (u32)height };
		extent.width = std::clamp( extent.width, m_SupportDetails.m_Capabilities.minImageExtent.width, m_SupportDetails.m_Capabilities.maxImageExtent.width );
		extent.height = std::clamp( extent.height, m_SupportDetails.m_Capabilities.minImageExtent.height, m_SupportDetails.m_Capabilities.maxImageExtent.height );

		return extent;
	}
}

//------------------------------------------------------------------------------------
void Engine::SwapChain::createSwapChain()
{
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat();
	VkPresentModeKHR presentMode = choosePresentMode();
	VkExtent2D swapExtent = chooseSwapExtent();
}

