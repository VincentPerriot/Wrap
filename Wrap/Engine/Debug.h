#pragma once

#include "../Utils/Common.h"

#include "vulkan/vulkan.h"

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
};