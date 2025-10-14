#pragma once

#include "../Maths/Matrix4.h"
#include "vulkan/vulkan_core.h"
#include "VulkanConstants.h"

namespace Engine
{
	struct CameraUBO
	{
		Maths::Matrix4 m_View;
		Maths::Matrix4 m_Proj;
	};

	struct ModelUBO
	{
		Maths::Matrix4 m_Model;
	};

	class UniformBuffer
	{
	public:
		UniformBuffer( VkDevice _device, VkPhysicalDevice _physDevice, VkDeviceSize _size );

		UniformBuffer( const UniformBuffer& _other ) = delete;
		UniformBuffer& operator=( const UniformBuffer& ) = delete;

		UniformBuffer( UniformBuffer&& _other ) = delete;
		UniformBuffer& operator=( UniformBuffer&& ) = delete;

		virtual ~UniformBuffer();

		void update( u32 _currentImage, const void* _data, size_t _size );

		inline std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT>& getBuffer();
		inline VkDeviceSize getUBOSize();
	private:
		void createBuffer();

		std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT> m_Buffers;
		std::array<VkDeviceMemory, MAX_FRAMES_IN_FLIGHT> m_MemoryArr;
		std::array<void*, MAX_FRAMES_IN_FLIGHT> m_MappedMemoryArr;

		VkDevice m_Device;
		VkPhysicalDevice m_PhysDevice;
		VkDeviceSize m_Size;
	};

	//------------------------------------------------------------------------------------
	inline std::array<VkBuffer, Engine::MAX_FRAMES_IN_FLIGHT>& Engine::UniformBuffer::getBuffer()
	{
		return m_Buffers;
	}

	//------------------------------------------------------------------------------------
	inline VkDeviceSize Engine::UniformBuffer::getUBOSize()
	{
		return m_Size;
	}

} // end namespace Engine


