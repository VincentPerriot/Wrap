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
#include "UniformBuffer.h"
#include "VulkanConstants.h"

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

	class Renderer final
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
		void addMesh( Scene::Mesh _mesh );
		void removeMesh( Scene::Mesh& _mesh );

		void drawFrames();

		void updateCameraUBO( Maths::Matrix4 _view, f32 _fov, f32 _near, f32 _far );
		void updateModelUBOs( size_t _pos, Maths::Matrix4 _model );

		void init( GLFWwindow* _pWindow );

	private:
		void createInstance();
		void setupPhysicalDevice();
		void createLogicalDevice();
		void createSurface( GLFWwindow* _pWindow );
		void createDescriptorSetLayout();
		void createDescriptorPool();
		void createDescriptorSets();
		void createGraphicsPipeline( bool _compile );
		void createCommandPool();
		void createCommandBuffers();
		void createSyncObjects();

		void updateDescriptors();

		void recordCommandBuffer( u32 _imageIndex );

		void onShaderModification( const std::filesystem::path& _path );

		void destroyBuffersFreeMemory();

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
		VkDescriptorSetLayout m_DescriptorSetLayout;
		VkDescriptorPool m_DescriptorPool;
		std::vector<VkDescriptorSet> m_DescriptorSets;
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

		std::vector<VkBuffer> m_VertexBuffers;
		std::vector<VkDeviceMemory> m_VertexBuffersMemory;

		std::vector<VkBuffer> m_IndexBuffers;
		std::vector<VkDeviceMemory> m_IndexBuffersMemory;

		std::unique_ptr<UniformBuffer> m_CameraUBO;
		std::vector<std::unique_ptr<UniformBuffer>> m_ModelUBOs;
	};
} // End Namespace Engine