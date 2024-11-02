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
	setUpDebugMessenger();
}

//----------------------------------------------------------------------------------
Renderer::~Renderer()
{
	if ( enableValidationLayers )
		//destroyDebugUtilsMessenger( m_Instance, m_DebugMessenger, nullptr );
		
	vkDestroyInstance( m_Instance, nullptr );
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
void Renderer::setUpDebugMessenger()
{
	if ( !enableValidationLayers )
		return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.pNext = nullptr,
		.flags = 0,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
		.pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT)VulkanDebug::debugCallback,
		.pUserData = nullptr,
	};

	assert( createDebugUtilsMessenger( m_Instance, &createInfo, nullptr, m_DebugMessenger ) == VK_SUCCESS );
}

//----------------------------------------------------------------------------------
std::vector<const char*> Renderer::getRequiredExtensions()
{
	u32 glfwExtCount = 0;
	const char** glfwExt;

	glfwExt = glfwGetRequiredInstanceExtensions( &glfwExtCount );

	std::vector<const char*> extensions( glfwExt, glfwExt + glfwExtCount );

	if ( enableValidationLayers )
		extensions.emplace_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );

	return extensions;
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

//----------------------------------------------------------------------------------
VkResult Renderer::createDebugUtilsMessenger( VkInstance& _instance, const VkDebugUtilsMessengerCreateInfoEXT* _createInfo,
	const VkAllocationCallbacks* _cbAlloc, VkDebugUtilsMessengerEXT& _messenger )
{
	auto fn = (PFN_vkCreateDebugUtilsMessengerEXT)( vkGetInstanceProcAddr( _instance, "vkCreateDebugUtilsMessengerEXT" ) );

	if ( fn )
		return fn( _instance, _createInfo, _cbAlloc, &_messenger );
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

//----------------------------------------------------------------------------------
void Renderer::destroyDebugUtilsMessenger( VkInstance& _instance, VkDebugUtilsMessengerEXT& _messenger, const VkAllocationCallbacks* _cbAlloc )
{
	auto fn = (PFN_vkDestroyDebugUtilsMessengerEXT)( vkGetInstanceProcAddr( _instance, "vkCreateDebugUtilsMessengerEXT" ) );

	if ( fn )
		fn( _instance, _messenger, _cbAlloc );
}