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

		SwapChainSupportDetails getSupportDetails();
		u32 getExtentWidth();
		u32 getExtentHeight();
		VkRenderPass getRenderPass();
		VkFramebuffer getFrameBuffer( u32 _index );

		bool isAdequate();
		void recreateSwapChain();

		// Use this to recreate swapChain
		inline static bool m_BufferResized;

	private:
		void querySwapChainDetails();

		VkSurfaceFormatKHR chooseSwapSurfaceFormat();
		VkPresentModeKHR choosePresentMode();
		VkExtent2D chooseSwapExtent();

		void createSwapChain();
		void createImageViews();
		void createFrameBuffers();

		void cleanUpSwapChain();
		void createRenderPass();

		SwapChainSupportDetails m_SupportDetails;
		VkDevice& m_Device;
		VkPhysicalDevice& m_PhysicalDevice;

		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_ImageViews;

		VkSurfaceFormatKHR m_SelectedFormat;
		VkPresentModeKHR m_SelectedPresentMode;

		VkRenderPass m_RenderPass;

		std::vector<VkFramebuffer> m_FrameBuffers;

		VkSurfaceKHR& m_Surface;
		VkExtent2D m_Extent;
	};

} // End namespace Engine