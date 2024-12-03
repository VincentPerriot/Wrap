#pragma once

#include "../Utils/Common.h"
#include <vulkan/vulkan.h>

#include "../Platforms/Windows/Display.h"
#include "../Engine/Renderer.h"

namespace Config {
	inline u32 WINDOW_WIDTH = 1920;
	inline u32 WINDOW_HEIGHT = 1080;
}

class TriangleApp
{
public:
	TriangleApp();

	~TriangleApp();

	void run();

private:
	void initVulkan();

	void mainLoop();

	std::unique_ptr<Engine::Renderer> m_pRenderer;
};

