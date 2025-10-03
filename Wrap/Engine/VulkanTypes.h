#pragma once

#include "../Maths/Vector2.h"
#include "../Maths/Vector3.h"

#include "vulkan/vulkan_core.h"


namespace Engine {
	// TODO create proper interface for different vertices and their description
	struct Vertex final
	{
		Maths::Vector2 m_Pos;
		Maths::Vector3 m_Color;

		static VkVertexInputBindingDescription getBindingDescrition()
		{
			VkVertexInputBindingDescription bindingDesc{
				.binding = 0,
				.stride = sizeof( Vertex ),
				.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
			};

			return bindingDesc;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributesDescription()
		{
			std::array<VkVertexInputAttributeDescription, 2> attributesdesc;

			attributesdesc[0].binding = 0;
			attributesdesc[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributesdesc[0].location = 0;
			attributesdesc[0].offset = offsetof( Vertex, m_Pos );

			attributesdesc[1].binding = 0;
			attributesdesc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributesdesc[1].location = 1;
			attributesdesc[1].offset = offsetof( Vertex, m_Color );

			return attributesdesc;
		}
	};
} // end namespace Engine