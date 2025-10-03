#pragma once
#pragma once

#include "../Utils/Common.h"
#include "Mesh.h"

namespace Scene {
	class Scene
	{
	private:
		std::array<Mesh, 4> geometry;
	};

} // end namespace Scene
