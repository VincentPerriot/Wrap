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
		AppScene();
		~AppScene();

		void start();
		void update( Engine::Renderer& _renderer );

	private:
		void addGeometry();

		::Scene::MeshHandle m_Triangle1;
		::Scene::MeshHandle m_Triangle2;
	};
} // end namespace App::ModelApp


