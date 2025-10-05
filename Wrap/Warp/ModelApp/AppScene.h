#pragma once
#pragma once

#include "../../Scene/Scene.h"

#include "../../Scene/PrimitiveMeshes/Triangle.h"
#include "../../Scene/PrimitiveMeshes/Quad.h"

namespace App::ModelApp
{
	class AppScene : Scene::Scene
	{
	public:
		AppScene( Engine::Renderer& _renderer );
		~AppScene();

		void start();
		void update();

	private:
		void addGeometry();

		::Scene::MeshHandle m_Triangle1;
		::Scene::MeshHandle m_Triangle2;

		f32 m_SceneTime{ 0.0f };
		std::chrono::time_point<std::chrono::steady_clock> m_LastTimeUpdate;
		Engine::Renderer& m_Renderer;

		bool m_RemoveTest{ true };
	};
} // end namespace App::ModelApp


