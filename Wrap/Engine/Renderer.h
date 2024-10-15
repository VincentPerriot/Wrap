#pragma once

#include "../Utils/Common.h"

#include "vulkan/vulkan.h"

constexpr std::array<const char*, 1 > validation_layers = {
	"VK_LAYER_KHRONOS_validation"
};

class Renderer
{
public:
	Renderer();
	~Renderer();

	void vulkanInit();
	void cleanUp();

	bool checkValidationSupport();

private:
	void createInstance();

	VkInstance m_Instance;
};