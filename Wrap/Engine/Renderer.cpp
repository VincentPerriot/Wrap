#include "Renderer.h"
#include "ShaderModule.h"
#include "RuntimeShaderCompiler.h"
#include "VulkanTypes.h"
#include "VulkanMemory.h"
#include "../Utils/Common.h"
#include "../Maths/Matrix4.h"

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
		vkDeviceWaitIdle( m_LogicalDevice );

		destroyBuffersFreeMemory();

		vkDestroyPipeline( m_LogicalDevice, m_GraphicsPipeline, nullptr );
		vkDestroyPipelineLayout( m_LogicalDevice, m_PipelineLayout, nullptr );
		m_Swapchain.reset();

		vkDestroyDescriptorPool( m_LogicalDevice, m_DescriptorPool, nullptr );
		vkDestroyDescriptorSetLayout( m_LogicalDevice, m_DescriptorSetLayout, nullptr );

		if ( enableValidationLayers )
			destroyDebugUtilsMessenger( m_Instance, m_DebugMessenger, nullptr );

		vkDestroyCommandPool( m_LogicalDevice, m_CommandPool, nullptr );

		for ( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
		{
			vkDestroySemaphore( m_LogicalDevice, m_ImageAvailableSemaphores[i], nullptr );
			vkDestroySemaphore( m_LogicalDevice, m_RenderFinishedSemaphores[i], nullptr );
			vkDestroyFence( m_LogicalDevice, m_inFlightFences[i], nullptr );
		}

		vkDestroyDevice( m_LogicalDevice, nullptr );

		vkDestroySurfaceKHR( m_Instance, m_Surface, nullptr );
		vkDestroyInstance( m_Instance, nullptr );

		m_PhysicalDevice = VK_NULL_HANDLE;

		m_ShaderWatcher.reset();
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
		m_ShaderWatcher = std::make_unique<FileWatcher>( "./Shaders", [this]( const std::filesystem::path& _path ) { this->onShaderModification( _path ); } );

		m_CameraUBO = std::make_unique<UniformBuffer>( m_LogicalDevice, m_PhysicalDevice, sizeof( CameraUBO ) );
		createDescriptorSetLayout();
		createDescriptorPool();
		createDescriptorSets();
		createGraphicsPipeline( true );
		createCommandPool();
		createCommandBuffers();
		createSyncObjects();
	}

	//----------------------------------------------------------------------------------
	void Renderer::createInstance()
	{
		if ( enableValidationLayers )
		{
			assert( checkValidationSupport() );
		}

		const VkApplicationInfo appInfo{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = nullptr,
			.pApplicationName = "warp_engine_app",
			.applicationVersion = VK_MAKE_API_VERSION( 1, 0, 0, 0 ),
			.pEngineName = "wrap_engine",
			.engineVersion = VK_MAKE_API_VERSION( 1, 0, 0, 0 ),
			.apiVersion = VK_API_VERSION_1_3
		};

		auto glfwExtensions = getRequiredExtensions();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.pNext = nullptr,
			.flags = 0,
			.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
			.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
			.pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT)&VulkanDebug::debugCallback,
			.pUserData = nullptr,
		};

		const VkInstanceCreateInfo createInfo{
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
		std::set<u32> unique_queues{ indices.m_Graphics.value(), indices.m_Present.value() };

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

		VkDeviceCreateInfo createInfo{
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
	void Renderer::createDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uboCameraBinding{
			.binding = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
			.pImmutableSamplers = nullptr
		};

		VkDescriptorSetLayoutBinding uboModelBinding{
			.binding = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
			.pImmutableSamplers = nullptr
		};

		std::array<VkDescriptorSetLayoutBinding, 2> bindings{ uboCameraBinding , uboModelBinding };

		VkDescriptorSetLayoutCreateInfo layoutInfo{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.bindingCount = static_cast<u32>( bindings.size() ),
			.pBindings = bindings.data()
		};

		VK_ASSERT( vkCreateDescriptorSetLayout( m_LogicalDevice, &layoutInfo, nullptr, &m_DescriptorSetLayout ) );
	}

	//----------------------------------------------------------------------------------
	void Renderer::createDescriptorPool()
	{
		VkDescriptorPoolSize CamPoolSize{
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = static_cast<u32>( MAX_FRAMES_IN_FLIGHT )
		};

		VkDescriptorPoolSize ModelsPoolSize{
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = static_cast<u32>( MAX_FRAMES_IN_FLIGHT * m_Meshes.size() )
		};

		std::array<VkDescriptorPoolSize, 2> poolSizes{ CamPoolSize, ModelsPoolSize };

		VkDescriptorPoolCreateInfo poolInfo{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.maxSets = MAX_FRAMES_IN_FLIGHT * ( 1 + m_Meshes.size() ),
			.poolSizeCount = static_cast<u32>( poolSizes.size() ),
			.pPoolSizes = poolSizes.data()
		};

		VK_ASSERT( vkCreateDescriptorPool( m_LogicalDevice, &poolInfo, nullptr, &m_DescriptorPool ) );
	}

	//----------------------------------------------------------------------------------
	void Renderer::createDescriptorSets()
	{
		std::vector<VkDescriptorSetLayout> layouts( MAX_FRAMES_IN_FLIGHT * ( 1 + m_Meshes.size() ), m_DescriptorSetLayout );

		VkDescriptorSetAllocateInfo allocInfo{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.pNext = nullptr,
			.descriptorPool = m_DescriptorPool,
			.descriptorSetCount = static_cast<u32>( layouts.size() ),
			.pSetLayouts = layouts.data()
		};

		m_DescriptorSets.resize( MAX_FRAMES_IN_FLIGHT * ( 1 + m_Meshes.size() ) );
		VK_ASSERT( vkAllocateDescriptorSets( m_LogicalDevice, &allocInfo, m_DescriptorSets.data() ) );

		for ( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
		{
			VkDescriptorBufferInfo bufferInfo{
				.buffer = m_CameraUBO->getBuffer()[i],
				.offset = 0,
				.range = m_CameraUBO->getUBOSize()
			};

			VkWriteDescriptorSet descWrite{
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = m_DescriptorSets[i],
				.dstBinding = 0,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.pImageInfo = nullptr,
				.pBufferInfo = &bufferInfo,
				.pTexelBufferView = nullptr
			};

			vkUpdateDescriptorSets( m_LogicalDevice, 1, &descWrite, 0, nullptr );
		}
	}

	//----------------------------------------------------------------------------------
	void Renderer::onShaderModification( const std::filesystem::path& _path )
	{
		std::lock_guard<std::mutex> guard( m_mutPipelineAccess );

		std::filesystem::path outdir{ "./Shaders/Compiled" };
		std::filesystem::path outfile{ outdir / _path.filename().concat( ".spv" ) };

		if ( RuntimeShaderCompiler::compile( _path, outfile ) )
		{
			vkDeviceWaitIdle( m_LogicalDevice );

			if ( m_GraphicsPipeline != VK_NULL_HANDLE )
				vkDestroyPipeline( m_LogicalDevice, m_GraphicsPipeline, nullptr );

			if ( m_PipelineLayout != VK_NULL_HANDLE )
				vkDestroyPipelineLayout( m_LogicalDevice, m_PipelineLayout, nullptr );

			createGraphicsPipeline( false );
		}
	}

	//----------------------------------------------------------------------------------
	void Renderer::createGraphicsPipeline( bool _compile )
	{
		if ( _compile )
		{
			RuntimeShaderCompiler::compile( "./Shaders/main.vert", "./Shaders/Compiled/main.vert.spv" );
			RuntimeShaderCompiler::compile( "./Shaders/main.frag", "./Shaders/Compiled/main.frag.spv" );
		}

		auto pVertShader = std::make_unique<Engine::ShaderModule>( std::filesystem::path( "./Shaders/Compiled/main.vert.spv" ), m_LogicalDevice );
		auto pFragShader = std::make_unique<Engine::ShaderModule>( std::filesystem::path( "./Shaders/Compiled/main.frag.spv" ), m_LogicalDevice );

		VkShaderModule vertModule = pVertShader->getShaderModule();
		VkShaderModule fragModule = pFragShader->getShaderModule();

		VkPipelineShaderStageCreateInfo vertCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = vertModule,
			.pName = "main",
			.pSpecializationInfo = nullptr
		};

		VkPipelineShaderStageCreateInfo fragCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = fragModule,
			.pName = "main",
			.pSpecializationInfo = nullptr
		};

		std::array<VkPipelineShaderStageCreateInfo, 2> shaderStageCreateInfos{ vertCreateInfo, fragCreateInfo };

		std::vector<VkDynamicState> dynamicStates{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

		VkPipelineDynamicStateCreateInfo dynCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.dynamicStateCount = static_cast<u32>( dynamicStates.size() ),
			.pDynamicStates = dynamicStates.data()
		};

		auto bindingDesc = Vertex::getBindingDescrition();
		auto attributeDesc = Vertex::getAttributesDescription();

		VkPipelineVertexInputStateCreateInfo vertInputCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.vertexBindingDescriptionCount = 1,
			.pVertexBindingDescriptions = &bindingDesc,
			.vertexAttributeDescriptionCount = (u32)attributeDesc.size(),
			.pVertexAttributeDescriptions = attributeDesc.data()
		};

		VkPipelineInputAssemblyStateCreateInfo assemblyCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE
		};

		VkViewport viewport{
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>( m_Swapchain->getExtentWidth() ),
			.height = static_cast<float>( m_Swapchain->getExtentHeight() ),
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		};

		VkRect2D scissor{
			.offset = VkOffset2D{ 0, 0 },
			.extent = VkExtent2D{ m_Swapchain->getExtentWidth(), m_Swapchain->getExtentHeight() }
		};

		VkPipelineViewportStateCreateInfo viewPortStateCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.viewportCount = 1,
			.pViewports = &viewport,
			.scissorCount = 1,
			.pScissors = &scissor
		};

		VkPipelineRasterizationStateCreateInfo rasterizer{
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

		VkPipelineMultisampleStateCreateInfo multisampling{
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

		VkPipelineColorBlendAttachmentState colorBlendAttachment{
			.blendEnable = VK_TRUE,
			.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
			.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			.colorBlendOp = VK_BLEND_OP_ADD,
			.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
			.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			.alphaBlendOp = VK_BLEND_OP_ADD,
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		};

		VkPipelineColorBlendStateCreateInfo colorBlending{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.logicOpEnable = VK_FALSE,
			.logicOp = VK_LOGIC_OP_COPY,
			.attachmentCount = (u32)1,
			.pAttachments = &colorBlendAttachment,
			.blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f }
		};

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.setLayoutCount = 0,
			.pSetLayouts = nullptr,
			.pushConstantRangeCount = 0,
			.pPushConstantRanges = nullptr
		};

		VK_ASSERT( vkCreatePipelineLayout( m_LogicalDevice, &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout ) );

		VkGraphicsPipelineCreateInfo pipelineCreateInfo{
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
			.renderPass = m_Swapchain->getRenderPass(),
			.subpass = 0,
			.basePipelineHandle = VK_NULL_HANDLE,
			.basePipelineIndex = -1
		};

		VK_ASSERT( vkCreateGraphicsPipelines( m_LogicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_GraphicsPipeline ) );

		pVertShader.reset();
		pFragShader.reset();
	}

	//----------------------------------------------------------------------------------
	void Renderer::createCommandPool()
	{
		QueueFamilyIndices queueFamIndices = findQueueFamilies();

		VkCommandPoolCreateInfo poolInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = queueFamIndices.m_Graphics.value()
		};

		VK_ASSERT( vkCreateCommandPool( m_LogicalDevice, &poolInfo, nullptr, &m_CommandPool ) );
	}

	//----------------------------------------------------------------------------------
	void Renderer::createCommandBuffers()
	{
		m_CommandBuffers.resize( MAX_FRAMES_IN_FLIGHT );

		VkCommandBufferAllocateInfo allocInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = nullptr,
			.commandPool = m_CommandPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = (u32)m_CommandBuffers.size()
		};

		VK_ASSERT( vkAllocateCommandBuffers( m_LogicalDevice, &allocInfo, m_CommandBuffers.data() ) );
	}

	//----------------------------------------------------------------------------------
	void Renderer::createSyncObjects()
	{
		m_ImageAvailableSemaphores.resize( MAX_FRAMES_IN_FLIGHT );
		m_RenderFinishedSemaphores.resize( MAX_FRAMES_IN_FLIGHT );
		m_inFlightFences.resize( MAX_FRAMES_IN_FLIGHT );

		for ( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
		{
			VkSemaphoreCreateInfo semaphoreInfo{
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0
			};

			VkFenceCreateInfo fenceInfo{
				.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
				.pNext = nullptr,
				.flags = VK_FENCE_CREATE_SIGNALED_BIT
			};

			VK_ASSERT( vkCreateSemaphore( m_LogicalDevice, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i] ) );
			VK_ASSERT( vkCreateSemaphore( m_LogicalDevice, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i] ) );
			VK_ASSERT( vkCreateFence( m_LogicalDevice, &fenceInfo, nullptr, &m_inFlightFences[i] ) );
		}
	}

	//----------------------------------------------------------------------------------
	void Renderer::recordCommandBuffer( u32 _imageIndex )
	{
		std::lock_guard<std::mutex> guard( m_mutPipelineAccess );

		VkCommandBufferBeginInfo beginInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = nullptr,
			.flags = 0,
			.pInheritanceInfo = nullptr
		};

		VK_ASSERT( vkBeginCommandBuffer( m_CommandBuffers[m_CurrentFrame], &beginInfo ) );

		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		VkRenderPassBeginInfo passInfo{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.pNext = nullptr,
			.renderPass = m_Swapchain->getRenderPass(),
			.framebuffer = m_Swapchain->getFrameBuffer( _imageIndex ),
			.renderArea = VkRect2D{.offset = {0, 0}, .extent = VkExtent2D { m_Swapchain->getExtentWidth(), m_Swapchain->getExtentHeight() } },
			.clearValueCount = 1,
			.pClearValues = &clearColor
		};

		vkCmdBeginRenderPass( m_CommandBuffers[m_CurrentFrame], &passInfo, VK_SUBPASS_CONTENTS_INLINE );

		vkCmdBindPipeline( m_CommandBuffers[m_CurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline );

		VkViewport viewport{
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>( m_Swapchain->getExtentWidth() ),
			.height = static_cast<float>( m_Swapchain->getExtentHeight() ),
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		};
		vkCmdSetViewport( m_CommandBuffers[m_CurrentFrame], 0, 1, &viewport );

		VkRect2D scissor{
			.offset = VkOffset2D{ 0, 0 },
			.extent = VkExtent2D{ m_Swapchain->getExtentWidth(), m_Swapchain->getExtentHeight() }
		};
		vkCmdSetScissor( m_CommandBuffers[m_CurrentFrame], 0, 1, &scissor );

		for ( size_t i = 0; i < m_Meshes.size(); i++ )
		{
			std::vector<VkDeviceSize> offsets( m_Meshes.size() );
			vkCmdBindVertexBuffers( m_CommandBuffers[m_CurrentFrame], 0, 1, &m_VertexBuffers[i], offsets.data() );
			vkCmdBindIndexBuffer( m_CommandBuffers[m_CurrentFrame], m_IndexBuffers[i], 0, VK_INDEX_TYPE_UINT16 );

			u32 indexCount = static_cast<u32>( m_Meshes[i].getIndices().size() );
			vkCmdDrawIndexed( m_CommandBuffers[m_CurrentFrame], indexCount, 1, 0, 0, 0 );
		}

		vkCmdEndRenderPass( m_CommandBuffers[m_CurrentFrame] );
		VK_ASSERT( vkEndCommandBuffer( m_CommandBuffers[m_CurrentFrame] ) );
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
			// Here, make it a requirement and create swapchain with VK_SHARING_MODE_EXCLUSIVE
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
	void Renderer::loadMeshes( std::vector<Scene::Mesh> _meshes )
	{
		m_Meshes = _meshes;

		size_t numMeshes = m_Meshes.size();
		m_VertexBuffers.resize( numMeshes );
		m_IndexBuffers.resize( numMeshes );
		m_VertexBuffersMemory.resize( numMeshes );
		m_IndexBuffersMemory.resize( numMeshes );

		for ( size_t i = 0; i < numMeshes; i++ )
		{
			VulkanMemory::createMeshVertexBuffer( m_LogicalDevice, m_PhysicalDevice, m_Meshes[i], m_VertexBuffers[i],
				m_VertexBuffersMemory[i], m_CommandPool, m_GraphicsQueue );
			VulkanMemory::createMeshIndexBuffer( m_LogicalDevice, m_PhysicalDevice, m_Meshes[i], m_IndexBuffers[i],
				m_IndexBuffersMemory[i], m_CommandPool, m_GraphicsQueue );
		}

		m_ModelUBOs.resize( m_Meshes.size() );
		for ( auto& pUBO : m_ModelUBOs )
		{
			pUBO = std::make_unique<UniformBuffer>( m_LogicalDevice, m_PhysicalDevice, sizeof( ModelUBO ) );
		}
	}

	//----------------------------------------------------------------------------------
	void Renderer::removeMesh( Scene::Mesh& _mesh )
	{
		auto it = std::ranges::find( m_Meshes, _mesh );
		if ( it != m_Meshes.end() )
		{
			auto index = (size_t)std::distance( m_Meshes.begin(), it );

			vkFreeMemory( m_LogicalDevice, m_VertexBuffersMemory[index], nullptr );
			vkDestroyBuffer( m_LogicalDevice, m_VertexBuffers[index], nullptr );
			vkFreeMemory( m_LogicalDevice, m_IndexBuffersMemory[index], nullptr );
			vkDestroyBuffer( m_LogicalDevice, m_IndexBuffers[index], nullptr );

			m_Meshes.erase( m_Meshes.begin() + index );
			m_VertexBuffers.erase( m_VertexBuffers.begin() + index );
			m_VertexBuffersMemory.erase( m_VertexBuffersMemory.begin() + index );
			m_IndexBuffers.erase( m_IndexBuffers.begin() + index );
			m_IndexBuffersMemory.erase( m_IndexBuffersMemory.begin() + index );

			m_ModelUBOs.erase( m_ModelUBOs.begin() + index );
		}
	}

	//----------------------------------------------------------------------------------
	void Renderer::drawFrames()
	{
		vkWaitForFences( m_LogicalDevice, 1, &m_inFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX );

		u32 imageIndex;

		VkResult res = vkAcquireNextImageKHR( m_LogicalDevice, m_Swapchain->m_VkSwapChain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex );

		if ( m_Swapchain->m_BufferResized || res == VK_ERROR_OUT_OF_DATE_KHR )
		{
			m_Swapchain->m_BufferResized = false;
			createSyncObjects();
			m_Swapchain->recreateSwapChain();
			return;
		}

		vkResetFences( m_LogicalDevice, 1, &m_inFlightFences[m_CurrentFrame] );

		vkResetCommandBuffer( m_CommandBuffers[m_CurrentFrame], 0 );
		recordCommandBuffer( imageIndex );

		std::array<VkSemaphore, 1> waitSemaphores{ m_ImageAvailableSemaphores[m_CurrentFrame] };
		std::array<VkSemaphore, 1> signalSemaphores{ m_RenderFinishedSemaphores[m_CurrentFrame] };
		std::array<VkPipelineStageFlags, 1> waitStages{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitInfo{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pNext = nullptr,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = waitSemaphores.data(),
			.pWaitDstStageMask = waitStages.data(),
			.commandBufferCount = 1,
			.pCommandBuffers = &m_CommandBuffers[m_CurrentFrame],
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = signalSemaphores.data()
		};

		VK_ASSERT( vkQueueSubmit( m_GraphicsQueue, 1, &submitInfo, m_inFlightFences[m_CurrentFrame] ) );

		std::array<VkSwapchainKHR, 1> swapChains{ m_Swapchain->m_VkSwapChain };
		VkPresentInfoKHR presentInfo{
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.pNext = nullptr,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = signalSemaphores.data(),
			.swapchainCount = 1,
			.pSwapchains = swapChains.data(),
			.pImageIndices = &imageIndex,
			.pResults = nullptr
		};

		vkQueuePresentKHR( m_PresentQueue, &presentInfo );

		m_CurrentFrame = ( m_CurrentFrame + 1 ) % MAX_FRAMES_IN_FLIGHT;
	}

	//----------------------------------------------------------------------------------
	void Renderer::updateCameraUBO( Maths::Matrix4 _view, f32 _fov, f32 _near, f32 _far )
	{
		auto aspect = m_Swapchain->getExtentWidth() / m_Swapchain->getExtentHeight();
		auto proj = Maths::Matrix4::Projection( _fov, aspect, _near, _far );

		CameraUBO camera{
			.m_View = _view,
			.m_Proj = proj
		};

		m_CameraUBO->update( m_CurrentFrame, &camera, sizeof( CameraUBO ) );
	}

	//----------------------------------------------------------------------------------
	void Renderer::updateModelUBOs( size_t _pos, Maths::Matrix4 _model )
	{
		ModelUBO model{
			.m_Model = _model
		};

		m_ModelUBOs[_pos]->update( m_CurrentFrame, &model, sizeof( ModelUBO ) );
	}

	//----------------------------------------------------------------------------------
	void Renderer::destroyBuffersFreeMemory()
	{
		for ( size_t i = 0; i < m_VertexBuffers.size(); i++ )
		{
			vkDestroyBuffer( m_LogicalDevice, m_VertexBuffers[i], nullptr );
		}
		for ( size_t i = 0; i < m_VertexBuffersMemory.size(); i++ )
		{
			vkFreeMemory( m_LogicalDevice, m_VertexBuffersMemory[i], nullptr );
		}
		for ( size_t i = 0; i < m_IndexBuffers.size(); i++ )
		{
			vkDestroyBuffer( m_LogicalDevice, m_IndexBuffers[i], nullptr );
		}
		for ( size_t i = 0; i < m_IndexBuffersMemory.size(); i++ )
		{
			vkFreeMemory( m_LogicalDevice, m_IndexBuffersMemory[i], nullptr );
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