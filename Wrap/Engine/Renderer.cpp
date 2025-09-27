#include "Renderer.h"
#include "ShaderModule.h"
#include "RuntimeShaderCompiler.h"

#ifdef NDEBUG
const bool enableValidationLayers = true;
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
		vkDestroyPipeline( m_LogicalDevice, m_GraphicsPipeline, nullptr );
		vkDestroyPipelineLayout( m_LogicalDevice, m_PipelineLayout, nullptr );
		vkDestroyRenderPass( m_LogicalDevice, m_RenderPass, nullptr );
		m_Swapchain.reset();

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

		m_Swapchain = std::make_unique<Engine::SwapChain>( m_PhysicalDevice, m_LogicalDevice, m_Surface );
		assert( isDeviceSuitable() );

		createRenderPass();
		createGraphicsPipeline();
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
			.pApplicationName = "warp_engine_app",
			.applicationVersion = VK_MAKE_API_VERSION( 1, 0, 0, 0 ),
			.pEngineName = "wrap_engine",
			.engineVersion = VK_MAKE_API_VERSION( 1, 0, 0, 0 ),
			.apiVersion = VK_API_VERSION_1_3
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

		assert( m_PhysicalDevice != VK_NULL_HANDLE );
	}

	//----------------------------------------------------------------------------------
	void Renderer::createLogicalDevice()
	{
		QueueFamilyIndices indices = findQueueFamilies();
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
			.enabledExtensionCount = static_cast<u32>( device_extensions.size() ),
			.ppEnabledExtensionNames = device_extensions.data(),
			.pEnabledFeatures = &features
		};

		VK_ASSERT( vkCreateDevice( m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice ) );

		vkGetDeviceQueue( m_LogicalDevice, indices.m_Graphics.value(), 0, &m_GraphicsQueue );
		vkGetDeviceQueue( m_LogicalDevice, indices.m_Present.value(), 0, &m_PresentQueue );
	}

	//----------------------------------------------------------------------------------
	void Renderer::createSurface( GLFWwindow* _pWindow )
	{
		assert( _pWindow );
		VK_ASSERT( glfwCreateWindowSurface( m_Instance, _pWindow, nullptr, &m_Surface ) );
	}

	//----------------------------------------------------------------------------------
	void Renderer::createGraphicsPipeline()
	{
		RuntimeShaderCompiler::compile( "./Shaders/main.vert", "./Shaders/Compiled/main.vert.spv" );
		RuntimeShaderCompiler::compile( "./Shaders/main.frag", "./Shaders/Compiled/main.frag.spv" );

		auto pVertShader = std::make_unique<Engine::ShaderModule>( std::filesystem::path( "./Shaders/Compiled/main.vert.spv" ), m_LogicalDevice );
		auto pFragShader = std::make_unique<Engine::ShaderModule>( std::filesystem::path( "./Shaders/Compiled/main.frag.spv" ), m_LogicalDevice );

		VkShaderModule vertModule = pVertShader->getShaderModule();
		VkShaderModule fragModule = pFragShader->getShaderModule();

		VkPipelineShaderStageCreateInfo vertCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = vertModule,
			.pName = "main",
			.pSpecializationInfo = nullptr
		};

		VkPipelineShaderStageCreateInfo fragCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = fragModule,
			.pName = "main",
			.pSpecializationInfo = nullptr
		};

		std::array<VkPipelineShaderStageCreateInfo, 2> shaderStageCreateInfos = { vertCreateInfo, fragCreateInfo };

		std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

		VkPipelineDynamicStateCreateInfo dynCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.dynamicStateCount = static_cast<u32>( dynamicStates.size() ),
			.pDynamicStates = dynamicStates.data()
		};

		VkPipelineVertexInputStateCreateInfo vertInputCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.vertexBindingDescriptionCount = 0,
			.pVertexBindingDescriptions = nullptr,
			.vertexAttributeDescriptionCount = 0,
			.pVertexAttributeDescriptions = nullptr
		};

		VkPipelineInputAssemblyStateCreateInfo assemblyCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE
		};

		VkViewport viewport = {
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>( m_Swapchain->getExtentWidth() ),
			.height = static_cast<float>( m_Swapchain->getExtentHeight() ),
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		};

		VkRect2D scissor = {
			.offset = VkOffset2D{ (u32)0, (u32)0 },
			.extent = VkExtent2D{ m_Swapchain->getExtentWidth(), m_Swapchain->getExtentHeight() }
		};

		VkPipelineViewportStateCreateInfo viewPortStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.viewportCount = 1,
			.pViewports = &viewport,
			.scissorCount = 1,
			.pScissors = &scissor
		};

		VkPipelineRasterizationStateCreateInfo rasterizer = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.depthClampEnable = VK_FALSE,
			.rasterizerDiscardEnable = VK_FALSE,
			.polygonMode = VK_POLYGON_MODE_FILL,
			.cullMode = VK_CULL_MODE_BACK_BIT,
			.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
			.depthBiasEnable = VK_FALSE,
			.depthBiasConstantFactor = 0.0f,
			.depthBiasClamp = 0.0f,
			.depthBiasSlopeFactor = 0.0f,
			.lineWidth = 1.0f
		};

		VkPipelineMultisampleStateCreateInfo multisampling = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
			.sampleShadingEnable = VK_FALSE,
			.minSampleShading = 1.0f,
			.pSampleMask = nullptr,
			.alphaToCoverageEnable = VK_FALSE,
			.alphaToOneEnable = VK_FALSE
		};

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {
			.blendEnable = VK_TRUE,
			.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
			.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			.colorBlendOp = VK_BLEND_OP_ADD,
			.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
			.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			.alphaBlendOp = VK_BLEND_OP_ADD,
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		};

		VkPipelineColorBlendStateCreateInfo colorBlending = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.logicOpEnable = VK_FALSE,
			.logicOp = VK_LOGIC_OP_COPY,
			.attachmentCount = (u32)1,
			.pAttachments = &colorBlendAttachment,
			.blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f }
		};

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.setLayoutCount = 0,
			.pSetLayouts = nullptr,
			.pushConstantRangeCount = 0,
			.pPushConstantRanges = nullptr
		};

		VK_ASSERT( vkCreatePipelineLayout( m_LogicalDevice, &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout ) );

		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.stageCount = 2,
			.pStages = shaderStageCreateInfos.data(),
			.pVertexInputState = &vertInputCreateInfo,
			.pInputAssemblyState = &assemblyCreateInfo,
			.pTessellationState = nullptr,
			.pViewportState = &viewPortStateCreateInfo,
			.pRasterizationState = &rasterizer,
			.pMultisampleState = &multisampling,
			.pDepthStencilState = nullptr,
			.pColorBlendState = &colorBlending,
			.pDynamicState = &dynCreateInfo,
			.layout = m_PipelineLayout,
			.renderPass = m_RenderPass,
			.subpass = 0,
			.basePipelineHandle = VK_NULL_HANDLE,
			.basePipelineIndex = -1
		};

		VK_ASSERT( vkCreateGraphicsPipelines( m_LogicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_GraphicsPipeline ) );
	}

	//----------------------------------------------------------------------------------
	void Renderer::createRenderPass()
	{
		VkAttachmentDescription colorAttachment = {
			.flags = 0,
			.format = m_Swapchain->getImageFormat().format,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		};

		VkAttachmentReference colorAttachmentRef = {
			.attachment = 0,
			.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};

		VkSubpassDescription subpass = {
			.flags = 0,
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.inputAttachmentCount = 0,
			.pInputAttachments = nullptr,
			.colorAttachmentCount = 1,
			.pColorAttachments = &colorAttachmentRef,
			.pResolveAttachments = nullptr,
			.pDepthStencilAttachment = nullptr,
			.preserveAttachmentCount = 0,
			.pPreserveAttachments = nullptr
		};

		VkRenderPassCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.attachmentCount = 1,
			.pAttachments = &colorAttachment,
			.subpassCount = 1,
			.pSubpasses = &subpass,
			.dependencyCount = 0,
			.pDependencies = nullptr
		};

		VK_ASSERT( vkCreateRenderPass( m_LogicalDevice, &createInfo, nullptr, &m_RenderPass ) );
	}

	//----------------------------------------------------------------------------------
	QueueFamilyIndices Renderer::findQueueFamilies()
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

			// According to Vulkan Doc -> explicitly prefer a physical device that supports drawing and presentation in the same queue 
			// Here we make it a requirement and create our swapchain with VK_SHARING_MODE_EXCLUSIVE
			if ( indices.verifyGraphics() && indices.m_Present == indices.m_Graphics )
				break;

			idx++;
		}

		assert( indices.verifyGraphics() && indices.m_Present == indices.m_Graphics );

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
	bool Renderer::checkDeviceExtensionsSupport()
	{
		u32 extensionCount;
		vkEnumerateDeviceExtensionProperties( m_PhysicalDevice, nullptr, &extensionCount, nullptr );

		std::vector<VkExtensionProperties> availableExt( extensionCount );
		vkEnumerateDeviceExtensionProperties( m_PhysicalDevice, nullptr, &extensionCount, availableExt.data() );

		for ( const auto& ext : device_extensions )
		{
			auto it = std::ranges::find_if( availableExt, [&ext]( VkExtensionProperties availableProp ) {
				return strcmp( ext, availableProp.extensionName ) == 0;
				} );
			if ( it == availableExt.end() ) return false;
		}

		return true;
	}

	//----------------------------------------------------------------------------------
	bool Renderer::isDeviceSuitable()
	{
		QueueFamilyIndices indices = findQueueFamilies();
		bool extensionSupport = checkDeviceExtensionsSupport();

		return indices.verifyGraphics() && extensionSupport && m_Swapchain->isAdequate();
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
	void Renderer::drawFrames()
	{
		if ( m_Swapchain->m_BufferResized )
		{
			m_Swapchain->m_BufferResized = false;
			m_Swapchain->recreateSwapChain();
		}
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

} // Namespace Engine