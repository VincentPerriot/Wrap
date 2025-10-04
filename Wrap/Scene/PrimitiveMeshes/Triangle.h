#pragma once
#include "../Mesh.h"

namespace Scene {
	namespace Primitives {
		class Triangle : public Mesh
		{
		public:
			Triangle( std::string_view _name );
			~Triangle();

		private:
			virtual void setVertices() override;
			virtual void setIndices() override;
		};
	} // end namespace Primitives
} // end namespace Scene
