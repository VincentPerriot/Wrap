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
		SwapChain( VkPhysicalDevice& _physicalDevice, VkSurfaceKHR& _surface );

		~SwapChain();

		SwapChain( const SwapChain& _other ) = delete;
		SwapChain& operator=( const SwapChain& ) = delete;

		SwapChain( SwapChain&& _other ) = delete;
		SwapChain& operator=( SwapChain&& ) = delete;

		SwapChainSupportDetails getSupportDetails() { return m_SupportDetails; }

		bool isAdequate();

	private:

		void querySwapChainDetails( VkPhysicalDevice& _physicalDevice, VkSurfaceKHR& _surface );

		VkSurfaceFormatKHR chooseSwapSurfaceFormat();
		VkPresentModeKHR choosePresentMode();
		VkExtent2D chooseSwapExtent();
		void createSwapChain();

		SwapChainSupportDetails m_SupportDetails;
	};

} // End namespace Engine