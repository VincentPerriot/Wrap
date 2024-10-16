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

	auto glfwExtensions = getRequiredExtensions();

	const VkInstanceCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = enableValidationLayers ? (u32)validation_layers.size() : 0,
		.ppEnabledLayerNames = enableValidationLayers ? validation_layers.data() : nullptr,
		.enabledExtensionCount = enableValidationLayers ? (u32)glfwExtensions.size() : 0,
		.ppEnabledExtensionNames = enableValidationLayers ? glfwExtensions.data() : nullptr
	};

	VkResult res = vkCreateInstance( &createInfo, nullptr, &m_Instance );

	assert( res == VK_SUCCESS );
}

//----------------------------------------------------------------------------------
std::vector<const char*> Renderer::getRequiredExtensions()
{
	u32 glfwExtCount = 0;
	const char** glfwExt;

	glfwExt = glfwGetRequiredInstanceExtensions( &glfwExtCount );

	// range based constructor
	std::vector<const char*> extensions( glfwExt, glfwExt + glfwExtCount );

	if ( enableValidationLayers )
		extensions.emplace_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );

	return extensions;
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
		auto it = std::ranges::find_if( availableLayers, [layer]( VkLayerProperties availableLayer ) {
			return strcmp( layer, availableLayer.layerName ); } );
		if ( it == availableLayers.end() ) return false;
	}

	return true;
}
