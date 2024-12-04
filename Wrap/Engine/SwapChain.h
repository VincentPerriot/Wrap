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
		SwapChain( VkPhysicalDevice& _physicalDevice, VkDevice& _device, VkSurfaceKHR& _surface );

		~SwapChain();

		SwapChain( const SwapChain& _other ) = delete;
		SwapChain& operator=( const SwapChain& ) = delete;

		SwapChain( SwapChain&& _other ) = delete;
		SwapChain& operator=( SwapChain&& ) = delete;

		VkSwapchainKHR m_VkSwapChain;

		SwapChainSupportDetails getSupportDetails() { return m_SupportDetails; }
		bool isAdequate();

		// Use this to recreate swapChain
		inline static bool m_BufferResized;

	private:
		void querySwapChainDetails( VkPhysicalDevice& _physicalDevice, VkSurfaceKHR& _surface );

		VkSurfaceFormatKHR chooseSwapSurfaceFormat();
		VkPresentModeKHR choosePresentMode();
		VkExtent2D chooseSwapExtent();
		void createSwapChain( VkSurfaceKHR& _surface );
		void createImageViews();

		SwapChainSupportDetails m_SupportDetails;
		VkDevice& m_Device;

		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_ImageViews;

		VkSurfaceFormatKHR m_SelectedFormat;
		VkPresentModeKHR m_SelectedPresentMode;
	};

} // End namespace Engine