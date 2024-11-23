#pragma once

#include "../Utils/Common.h"

#include "vulkan/vulkan.h"

namespace Engine {

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR m_Capabilities;
		std::vector<VkSurfaceFormatKHR> m_Formats;
		std::vector<VkPresentModeKHR> m_PresentModes;
	};

	class SwapChain {
	public:
		SwapChain( VkDevice& _device, VkPhysicalDevice& _physicalDevice, VkSurfaceKHR& _surface );

		~SwapChain();

		SwapChain( const SwapChain& _other ) = delete;
		SwapChain& operator=( const SwapChain& ) = delete;

		SwapChain( SwapChain&& _other ) = delete;
		SwapChain& operator=( SwapChain&& ) = delete;

	private:
		VkDevice m_Device;
		VkSurfaceKHR m_Surface;
		VkPhysicalDevice m_PhysicalDevice;
	};

} // End namespace Engine