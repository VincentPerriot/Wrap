#include "Renderer.h"

//----------------------------------------------------------------------------------
Renderer::Renderer()
{
	createInstance();
}

//----------------------------------------------------------------------------------
Renderer::~Renderer()
{

}

//----------------------------------------------------------------------------------
void Renderer::vulkanInit()
{

}

//----------------------------------------------------------------------------------
void Renderer::createInstance()
{
	const VkApplicationInfo appInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = nullptr,
		.pApplicationName = "test_triangle",
		.applicationVersion = VK_MAKE_API_VERSION( 1, 0, 0, 0 ),
		.pEngineName = "wrap_engine",
		.engineVersion = VK_MAKE_API_VERSION( 1, 0, 0, 0 ),
		.apiVersion = VK_API_VERSION_1_0
	};

	u32 glfwExtensionCount = 0;
	const std::vector<const char*> glfwExtensions{};

	const VkInstanceCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = nullptr,
		.enabledExtensionCount = glfwExtensionCount,
		.ppEnabledExtensionNames = glfwExtensions.data()
	};
}

