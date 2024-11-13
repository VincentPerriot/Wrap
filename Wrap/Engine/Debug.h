#pragma once

#include "../Utils/Common.h"

#include "vulkan/vulkan.h"


#define VK_ASSERT(x)				                                                                   \
	do {																                               \
		VkResult err = x;												                               \
		if ( err ) {													                               \
			std::cout << "Detected Vulkan error: " << VulkanDebug::to_string( err ) << std::endl;	   \
			abort();    												                               \
		}              													                               \
	} while ( 0 )														                               \
								    

class VulkanDebug {
public:

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT _msgSeverity,
		VkDebugUtilsMessageTypeFlagBitsEXT _msgType,
		const VkDebugUtilsMessengerCallbackDataEXT* _pCallBackData,
		void* pUserData )
	{
		std::cerr << " Validation layer: " << _pCallBackData->pMessage << std::endl;

		return VK_FALSE;
	}

	static const std::string to_string( VkResult result )
	{
		switch ( result )
		{
#define STR(r)   \
	case VK_##r: \
		return #r
			STR( NOT_READY );
			STR( TIMEOUT );
			STR( EVENT_SET );
			STR( EVENT_RESET );
			STR( INCOMPLETE );
			STR( ERROR_OUT_OF_HOST_MEMORY );
			STR( ERROR_OUT_OF_DEVICE_MEMORY );
			STR( ERROR_INITIALIZATION_FAILED );
			STR( ERROR_DEVICE_LOST );
			STR( ERROR_MEMORY_MAP_FAILED );
			STR( ERROR_LAYER_NOT_PRESENT );
			STR( ERROR_EXTENSION_NOT_PRESENT );
			STR( ERROR_FEATURE_NOT_PRESENT );
			STR( ERROR_INCOMPATIBLE_DRIVER );
			STR( ERROR_TOO_MANY_OBJECTS );
			STR( ERROR_FORMAT_NOT_SUPPORTED );
			STR( ERROR_FRAGMENTED_POOL );
			STR( ERROR_UNKNOWN );
			STR( ERROR_OUT_OF_POOL_MEMORY );
			STR( ERROR_INVALID_EXTERNAL_HANDLE );
			STR( ERROR_FRAGMENTATION );
			STR( ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS );
			STR( PIPELINE_COMPILE_REQUIRED );
			STR( ERROR_SURFACE_LOST_KHR );
			STR( ERROR_NATIVE_WINDOW_IN_USE_KHR );
			STR( SUBOPTIMAL_KHR );
			STR( ERROR_OUT_OF_DATE_KHR );
			STR( ERROR_INCOMPATIBLE_DISPLAY_KHR );
			STR( ERROR_VALIDATION_FAILED_EXT );
			STR( ERROR_INVALID_SHADER_NV );
#undef STR
		default:
			return "UNKNOWN_ERROR";
		}
	}
};