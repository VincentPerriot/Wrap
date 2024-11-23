#include "Renderer.h"

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

namespace Engine {
	//----------------------------------------------------------------------------------
	Renderer::Renderer()
	{
	}

	//----------------------------------------------------------------------------------
	Renderer::~Renderer()
	{
		vkDestroyDevice( m_LogicalDevice, nullptr );

		if ( enableValidationLayers )
			destroyDebugUtilsMessenger( m_Instance, m_DebugMessenger, nullptr );

		m_PhysicalDevice = VK_NULL_HANDLE;
		vkDestroySurfaceKHR( m_Instance, m_Surface, nullptr );
		vkDestroyInstance( m_Instance, nullptr );
	}

	//----------------------------------------------------------------------------------
	void Renderer::init( GLFWwindow* _pWindow )
	{
		createInstance();
		createSurface( _pWindow );
		setupPhysicalDevice();
		createLogicalDevice();
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

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.pNext = nullptr,
			.flags = 0,
			.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
			.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
			.pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT)&VulkanDebug::debugCallback,
			.pUserData = nullptr,
		};

		const VkInstanceCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = enableValidationLayers ? (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo : nullptr,
			.flags = 0,
			.pApplicationInfo = &appInfo,
			.enabledLayerCount = enableValidationLayers ? static_cast<u32>( validation_layers.size() ) : 0,
			.ppEnabledLayerNames = enableValidationLayers ? validation_layers.data() : nullptr,
			.enabledExtensionCount = enableValidationLayers ? static_cast<u32>( glfwExtensions.size() ) : 0,
			.ppEnabledExtensionNames = enableValidationLayers ? glfwExtensions.data() : nullptr
		};

		VK_ASSERT( vkCreateInstance( &createInfo, nullptr, &m_Instance ) );
		if ( enableValidationLayers )
			VK_ASSERT( createDebugUtilsMessenger( m_Instance, &debugCreateInfo, nullptr, &m_DebugMessenger ) );
	}

	//----------------------------------------------------------------------------------
	void Renderer::setupPhysicalDevice()
	{
		u32 numDevices;
		vkEnumeratePhysicalDevices( m_Instance, &numDevices, nullptr );
		std::vector<VkPhysicalDevice> devices( numDevices );
		vkEnumeratePhysicalDevices( m_Instance, &numDevices, devices.data() );

		u32 baseScore = 0;

		auto rateDevice = []( VkPhysicalDeviceProperties props, VkPhysicalDeviceFeatures features ) {
			u32 score = 0;
			switch ( props.deviceType )
			{
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				score += 10000;
				break;
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				score += 5000;
				break;
			default:
				break;
			}

			// TODO ADD MORE FEATURE CHECKS
			if ( features.geometryShader )
				score += 100;

			return score;
			};


		for ( const auto& device : devices )
		{
			VkPhysicalDeviceProperties props;
			vkGetPhysicalDeviceProperties( device, &props );
			VkPhysicalDeviceFeatures features;
			vkGetPhysicalDeviceFeatures( device, &features );

			u32 score = rateDevice( props, features );

			if ( score > baseScore )
			{
				baseScore = score;
				m_PhysicalDevice = device;
			}
		}

		auto isDeviceSuitable = [this]( VkPhysicalDevice _device ) {
			QueueFamilyIndices indices = findQueueFamilies( _device );
			return indices.verifyGraphics();
			};

		assert( m_PhysicalDevice != VK_NULL_HANDLE && isDeviceSuitable( m_PhysicalDevice ) );
	}

	//----------------------------------------------------------------------------------
	void Renderer::createLogicalDevice()
	{
		QueueFamilyIndices indices = findQueueFamilies( m_PhysicalDevice );
		std::set<u32> unique_queues = { indices.m_Graphics.value(), indices.m_Present.value() };

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos( unique_queues.size() );

		float queuePrio = 1.0f;
		std::ranges::transform( unique_queues, queueCreateInfos.begin(), [&queuePrio]( const auto queue )
			{
				return VkDeviceQueueCreateInfo{
				   .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				   .pNext = nullptr,
				   .flags = 0,
				   .queueFamilyIndex = queue,
				   .queueCount = 1,
				   .pQueuePriorities = &queuePrio
				};
			} );

		// Massive Struct -> GO back to it and pick features after more readings
		VkPhysicalDeviceFeatures features{};

		VkDeviceCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueCreateInfoCount = static_cast<u32>( queueCreateInfos.size() ),
			.pQueueCreateInfos = queueCreateInfos.data(),
			.enabledLayerCount = enableValidationLayers ? static_cast<u32>( validation_layers.size() ) : 0,
			.ppEnabledLayerNames = enableValidationLayers ? validation_layers.data() : nullptr,
			.pEnabledFeatures = &features
		};

		VK_ASSERT( vkCreateDevice( m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice ) );

		vkGetDeviceQueue( m_LogicalDevice, indices.m_Graphics.value(), 0, &m_GraphicsQueue );
		vkGetDeviceQueue( m_LogicalDevice, indices.m_Present.value(), 0, &m_GraphicsQueue );
	}

	//----------------------------------------------------------------------------------
	QueueFamilyIndices Renderer::findQueueFamilies( VkPhysicalDevice _device )
	{
		QueueFamilyIndices indices;

		u32 queueCount;
		vkGetPhysicalDeviceQueueFamilyProperties( m_PhysicalDevice, &queueCount, nullptr );
		std::vector<VkQueueFamilyProperties> queueFamilies( queueCount );
		vkGetPhysicalDeviceQueueFamilyProperties( m_PhysicalDevice, &queueCount, queueFamilies.data() );

		u32 idx = 0;

		for ( const auto& queueFamily : queueFamilies )
		{
			VkBool32 present = false;
			vkGetPhysicalDeviceSurfaceSupportKHR( m_PhysicalDevice, idx, m_Surface, &present );

			if ( present )
				indices.m_Present = idx;

			if ( queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT )
				indices.m_Graphics = idx;

			// According to Vulkan Doc ->  explicitly prefer a physical device that supports drawing and presentation in the same queue
			if ( indices.verifyGraphics() && indices.m_Present == indices.m_Graphics )
				break;

			idx++;
		}

		return indices;
	}

	//----------------------------------------------------------------------------------
	std::vector<const char*> Renderer::getRequiredExtensions()
	{
		u32 glfwExtCount = 0;
		const char** glfwExt;

		glfwExt = glfwGetRequiredInstanceExtensions( &glfwExtCount );

		std::vector<const char*> extensions( glfwExt, glfwExt + glfwExtCount );

		if ( enableValidationLayers )
		{
			extensions.push_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );
			extensions.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
		}

		return extensions;
	}

	//----------------------------------------------------------------------------------
	bool Renderer::checkValidationSupport()
	{
		u32 layerCount = 0;
		vkEnumerateInstanceLayerProperties( &layerCount, nullptr );

		std::vector<VkLayerProperties> availableLayers( layerCount );
		vkEnumerateInstanceLayerProperties( &layerCount, availableLayers.data() );

		for ( const auto& layer : validation_layers )
		{
			auto it = std::ranges::find_if( availableLayers, [&layer]( VkLayerProperties availableLayer ) {
				return strcmp( layer, availableLayer.layerName ) == 0; } );
			if ( it == availableLayers.end() ) return false;
		}

		return true;
	}

	//----------------------------------------------------------------------------------
	VkResult Renderer::createDebugUtilsMessenger( VkInstance _instance, const VkDebugUtilsMessengerCreateInfoEXT* _createInfo,
		const VkAllocationCallbacks* _cbAlloc, VkDebugUtilsMessengerEXT* _messenger )
	{
		auto fn = (PFN_vkCreateDebugUtilsMessengerEXT)( vkGetInstanceProcAddr( _instance, "vkCreateDebugUtilsMessengerEXT" ) );

		if ( fn )
			return fn( _instance, _createInfo, _cbAlloc, _messenger );
		else
			return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	//----------------------------------------------------------------------------------
	void Renderer::destroyDebugUtilsMessenger( VkInstance _instance, VkDebugUtilsMessengerEXT _messenger, const VkAllocationCallbacks* _cbAlloc )
	{
		auto fn = (PFN_vkDestroyDebugUtilsMessengerEXT)( vkGetInstanceProcAddr( _instance, "vkDestroyDebugUtilsMessengerEXT" ) );

		if ( fn )
			fn( _instance, _messenger, _cbAlloc );
	}

	//----------------------------------------------------------------------------------
	void Renderer::createSurface( GLFWwindow* _pWindow )
	{
		assert( _pWindow );
		VK_ASSERT( glfwCreateWindowSurface( m_Instance, _pWindow, nullptr, &m_Surface ) );
	}
} // Namespace Engine