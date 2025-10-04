#pragma once

#include <optional>
#include <mutex>

#include "../Utils/Common.h"
#include "../Utils/FileWatcher.h"
#include "../Scene/Mesh.h"

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

	constexpr u32 FRAMES_IN_FLIGHT = 2;

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
		void loadMeshes( std::vector<Scene::Mesh> _meshes );

		void drawFrames();

		// Temporary only running on GLFW
		void init( GLFWwindow* _pWindow );

	private:
		void createInstance();
		void setupPhysicalDevice();
		void createLogicalDevice();
		void createSurface( GLFWwindow* _pWindow );
		void createGraphicsPipeline( bool _compile );
		void createCommandPool();
		void createCommandBuffers();
		void createSyncObjects();

		void recordCommandBuffer( u32 _imageIndex );

		void onShaderModification( const std::filesystem::path& _path );

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

		VkCommandPool m_CommandPool;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_inFlightFences;

		u32 m_CurrentFrame{ 0 };

		std::unique_ptr<FileWatcher> m_ShaderWatcher;

		std::mutex m_mutPipelineAccess;

		std::vector<Scene::Mesh> m_Meshes;
	};
} // End Namespace Engine