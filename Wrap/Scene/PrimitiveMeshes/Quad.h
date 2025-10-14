#pragma once
#include "../Mesh.h"

namespace Scene {
	namespace Primitives {

		class Quad : public Mesh
		{
		public:
			Quad( std::string_view _name, const Maths::Matrix4& _modelMat = Maths::Matrix4::DefaultModelMatrix() );
			virtual ~Quad();

			virtual void setVertices() override;
			virtual void setIndices() override;
		};

	} // end namespace Primitives
} // end namespace Scene
