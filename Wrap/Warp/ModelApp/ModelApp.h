#pragma once

#include "../../Utils/Common.h"
#include <vulkan/vulkan.h>

#include "../../Platforms/Windows/Display.h"
#include "../../Engine/Renderer.h"
#include "../../Engine/VulkanTypes.h"
#include "../../Scene/BaseScene.h"
#include "AppScene.h"

namespace App {

	namespace ModelApp
	{
		class ModelApp
		{
		public:
			ModelApp();

			~ModelApp();

			void run();

		private:

			void mainLoop();

			void createScene();

			void initVulkan();

			std::unique_ptr<Engine::Renderer> m_pRenderer;
			std::unique_ptr<AppScene> m_AppScene;
		};
	} // end namespace Model App

} // end namespace App

