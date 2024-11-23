#pragma once

#include "../Utils/Common.h"
#include <vulkan/vulkan.h>

#include "../Platforms/Windows/Display.h"
#include "../Engine/Renderer.h"

class TriangleApp
{
public:
	TriangleApp();

	~TriangleApp();

	void run();

private:
	void initVulkan();

	void mainLoop();

	std::unique_ptr<Display> m_pDisplay;
	std::unique_ptr<Engine::Renderer> m_pRenderer;
};

