#pragma once
#include "../Mesh.h"

namespace Scene {
	namespace Primitives {

		class Quad : public Mesh
		{
			using Mesh::Mesh;

			virtual void setVertices() override;
			virtual void setIndices() override;
		};

	} // end namespace Primitives
} // end namespace Scene
