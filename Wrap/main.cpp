#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

#include "Platforms/Windows/Display.h"
#include "Utils/Common.h"

#include "Warp/TriangleApp.h"

#include "Engine/RuntimeShaderCompiler.h"

int main() {

	std::unique_ptr<TriangleApp> app = std::make_unique<TriangleApp>();

	app->run();

	return 0;
}