#pragma once

#include <optional>
#include "../Utils/Common.h"
#include "../Utils/FileWatcher.h"

#include "vulkan/vulkan.h"
#include "SwapChain.h"
#include "GLFW/glfw3.h"
#include "Debug.h"

namespace Engine {

	constexpr std::array<const char*, 1 > validation_layers = {
		"VK_LAYER_KHRONOS_validation"
	};

	constexpr std::array<const char*, 1 > device_extensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	struct QueueFamilyIndices {
		std::optional<u32> m_Graphics;
		std::optional<u32> m_Present;

		bool verifyGraphics() { return m_Graphics.has_value() && m_Present.has_value(); }
	};

	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		Renderer( const Renderer& _other ) = delete;
		Renderer& operator=( const Renderer& ) = delete;

		Renderer( Renderer&& _other ) = delete;
		Renderer& operator=( Renderer&& ) = delete;

		bool checkValidationSupport();

		void drawFrames();

		// Temporary only running on GLFW
		void init( GLFWwindow* _pWindow );

	private:
		void createInstance();
		void setupPhysicalDevice();
		void createLogicalDevice();
		void createSurface( GLFWwindow* _pWindow );

		void onShaderModification( const std::filesystem::path& _path );

		void createGraphicsPipeline();
		void createRenderPass();

		QueueFamilyIndices findQueueFamilies();

		VkResult createDebugUtilsMessenger( VkInstance _instance, const VkDebugUtilsMessengerCreateInfoEXT* _createInfo,
			const VkAllocationCallbacks* _cbAlloc, VkDebugUtilsMessengerEXT* _messenger );
		void destroyDebugUtilsMessenger( VkInstance _instance, VkDebugUtilsMessengerEXT _messenger, const VkAllocationCallbacks* _cbAlloc );

		std::vector<const char*> getRequiredExtensions();
		bool checkDeviceExtensionsSupport();
		bool isDeviceSuitable();

		std::unique_ptr<Engine::SwapChain> m_Swapchain;

		VkInstance m_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;
		VkPhysicalDevice m_PhysicalDevice;
		VkDevice m_LogicalDevice;
		VkSurfaceKHR m_Surface;

		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;

		VkPipeline m_GraphicsPipeline;
		VkPipelineLayout m_PipelineLayout;
		VkRenderPass m_RenderPass;

		std::unique_ptr<FileWatcher> m_ShaderWatcher;
	};
} // End Namespace Engine