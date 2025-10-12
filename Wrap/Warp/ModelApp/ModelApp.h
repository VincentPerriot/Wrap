#pragma once

#include "../../Utils/Common.h"
#include <vulkan/vulkan.h>

#include "../../Platforms/Windows/Display.h"
#include "../../Engine/Renderer.h"
#include "../../Engine/VulkanTypes.h"
#include "../../Scene/BaseScene.h"
#include "AppScene.h"

namespace App {

	static constexpr f32 FOV = 45.0f;
	static constexpr f32 NEAR_CLIP = 0.1f;
	static constexpr f32 FAR_CLIP = 10.0f;

	namespace ModelApp
	{
		class ModelApp
		{
		public:
			ModelApp();

			~ModelApp();

			void run();

		private:
			void initVulkan();

			void mainLoop();

			void createScene();

			std::unique_ptr<Engine::Renderer> m_pRenderer;
			std::unique_ptr<AppScene> m_AppScene;
		};
	} // end namespace Model App

} // end namespace App

