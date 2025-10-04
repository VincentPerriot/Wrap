#pragma once

#include "../../Utils/Common.h"
#include <vulkan/vulkan.h>

#include "../../Platforms/Windows/Display.h"
#include "../../Engine/Renderer.h"

namespace App {

	namespace TriangleApp
	{
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

	} // end namespace TriangleApp
} // end namespace App

