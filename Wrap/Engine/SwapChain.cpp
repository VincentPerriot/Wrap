#include "SwapChain.h"

//------------------------------------------------------------------------------------
Engine::SwapChain::SwapChain( VkDevice& _device, VkPhysicalDevice& _physicalDevice, VkSurfaceKHR& _surface )
	: m_Device( _device ), m_Surface( _surface ), m_PhysicalDevice( _physicalDevice )
{

}

//------------------------------------------------------------------------------------
Engine::SwapChain::~SwapChain()
{

}

