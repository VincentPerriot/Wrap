#pragma once

#include <optional>
#include "../Utils/Common.h"

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"
#include "Debug.h"

namespace Engine {

	constexpr std::array<const char*, 1 > validation_layers = {
		"VK_LAYER_KHRONOS_validation"
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

		bool checkValidationSupport();

		// Temporary only running on GLFW
		void init( GLFWwindow* _pWindow );

	private:
		void createInstance();
		void setupPhysicalDevice();
		void createLogicalDevice();
		void createSurface( GLFWwindow* _pWindow);

		QueueFamilyIndices findQueueFamilies( VkPhysicalDevice _device );

		VkResult createDebugUtilsMessenger( VkInstance _instance, const VkDebugUtilsMessengerCreateInfoEXT* _createInfo,
			const VkAllocationCallbacks* _cbAlloc, VkDebugUtilsMessengerEXT* _messenger );
		void destroyDebugUtilsMessenger( VkInstance _instance, VkDebugUtilsMessengerEXT _messenger, const VkAllocationCallbacks* _cbAlloc );

		std::vector<const char*> getRequiredExtensions();

		VkInstance m_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;
		VkPhysicalDevice m_PhysicalDevice;
		VkDevice m_LogicalDevice;
		VkSurfaceKHR m_Surface;

		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;
	};
} // End Namespace Engine