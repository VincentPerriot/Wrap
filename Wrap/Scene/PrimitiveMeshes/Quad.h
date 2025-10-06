#pragma once
#include "../Mesh.h"

namespace Scene {
	namespace Primitives {

		class Quad : public Mesh
		{
		public:
			Quad( std::string_view _name );
			~Quad();

			virtual void setVertices() override;
			virtual void setIndices() override;
		};

	} // end namespace Primitives
} // end namespace Scene
