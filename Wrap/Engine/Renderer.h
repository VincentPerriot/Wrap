#pragma once

#include <optional>
#include "../Utils/Common.h"

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"
#include "Debug.h"

constexpr std::array<const char*, 1 > validation_layers = {
	"VK_LAYER_KHRONOS_validation"
};

struct QueueFamilyIndices {
	std::optional<u32> m_graphics;

	bool verifyGraphics() { return m_graphics.has_value(); }
}; 



class Renderer
{
public:
	Renderer();
	~Renderer();

	bool checkValidationSupport();

private:
	void createInstance();
	void setupPhysicalDevice();

	QueueFamilyIndices findQueueFamilies( VkPhysicalDevice _device );

	VkResult createDebugUtilsMessenger( VkInstance _instance, const VkDebugUtilsMessengerCreateInfoEXT* _createInfo,
		const VkAllocationCallbacks* _cbAlloc, VkDebugUtilsMessengerEXT* _messenger );
	void destroyDebugUtilsMessenger( VkInstance _instance, VkDebugUtilsMessengerEXT _messenger, const VkAllocationCallbacks* _cbAlloc );

	std::vector<const char*> getRequiredExtensions();

	VkInstance m_Instance;
	VkDebugUtilsMessengerEXT m_DebugMessenger;
	VkPhysicalDevice m_PhysicalDevice;
};