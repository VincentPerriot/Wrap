#pragma once

#include "../Utils/Common.h"

#include "vulkan/vulkan.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

	void vulkanInit();

private:
	void createInstance();

	VkInstance m_Instance;
};