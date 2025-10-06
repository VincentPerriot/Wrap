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

		::Scene::MeshHandle m_Quad1;
		::Scene::MeshHandle m_Triangle2;
		::Scene::MeshHandle m_Quad3;
		::Scene::MeshHandle m_Triangle4;

		f32 m_SceneTime{ 0.0f };
		std::chrono::time_point<std::chrono::steady_clock> m_LastTimeUpdate;
		Engine::Renderer& m_Renderer;

		bool m_RemoveTest1{ true };
		bool m_RemoveTest2{ true };
		bool m_RemoveTest3{ true };
	};
} // end namespace App::ModelApp


