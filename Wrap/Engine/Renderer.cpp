#include "Renderer.h"

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

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
	if ( enableValidationLayers )
	{
		assert( checkValidationSupport() );
	}

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
		.enabledLayerCount = (u32)validation_layers.size(),
		.ppEnabledLayerNames = validation_layers.data(),
		.enabledExtensionCount = glfwExtensionCount,
		.ppEnabledExtensionNames = glfwExtensions.data()
	};

	VkResult res = vkCreateInstance( &createInfo, nullptr, &m_Instance );

	assert( res == VK_SUCCESS );
}

//----------------------------------------------------------------------------------
void Renderer::cleanUp()
{
	vkDestroyInstance( m_Instance, nullptr );
}

//----------------------------------------------------------------------------------
bool Renderer::checkValidationSupport()
{
	u32 layerCount = 0;
	vkEnumerateInstanceLayerProperties( &layerCount, nullptr );

	std::vector<VkLayerProperties> availableLayers( layerCount );
	vkEnumerateInstanceLayerProperties( &layerCount, availableLayers.data() );

	// Validation layers defined as constexpr in .h file
	for ( const auto& layer : validation_layers )
	{
		auto it = std::ranges::find( availableLayers, layer, &VkLayerProperties::layerName );
		if ( it == availableLayers.end() ) return false;
	}

	return true;
}
