#pragma once

#include "../Utils/Common.h"
#include <vulkan/vulkan.h>

#include "../Platforms/Windows/Display.h"
#include "../Engine/Renderer.h"
#include "../Engine/VulkanTypes.h"

class ModelApp
{
public:
	ModelApp();

	~ModelApp();

	void run();

private:
	void initVulkan();

	void mainLoop();

	std::unique_ptr<Engine::Renderer> m_pRenderer;
};
