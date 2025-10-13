#pragma once
#pragma once

#include "../../Scene/BaseScene.h"

#include "../../Scene/PrimitiveMeshes/Triangle.h"
#include "../../Scene/PrimitiveMeshes/Quad.h"

namespace App::ModelApp
{
	class AppScene : Scene::BaseScene
	{
	public:
		AppScene( Engine::Renderer& _renderer );
		~AppScene();

		void start();
		void update();

	private:
		void addGeometry();
		void setUpCamera();

		::Scene::MeshHandle m_Quad1;
		::Scene::MeshHandle m_Quad2;

		f32 m_SceneTime{ 0.0f };
		std::chrono::time_point<std::chrono::steady_clock> m_LastTimeUpdate;
		Engine::Renderer& m_Renderer;

		bool m_RemoveTest1{ true };
		bool m_RemoveTest2{ true };
		bool m_RemoveTest3{ true };

		Scene::Camera m_Camera;
		Scene::ProjectionSettings m_ProjectionSettings;
	};
} // end namespace App::ModelApp


