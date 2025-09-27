#pragma once

#include <fstream>
#include <filesystem>
#include "../Utils/Common.h"

#include "vulkan/vulkan.h"
#include "Debug.h"

namespace Engine {
	class ShaderModule {
		using Path = std::filesystem::path;

	public:
		ShaderModule( const Path& _filename, VkDevice& _device );

		ShaderModule( const ShaderModule& _other ) = delete;
		ShaderModule& operator=( const ShaderModule& ) = delete;

		ShaderModule( ShaderModule&& _other ) = delete;
		ShaderModule& operator=( ShaderModule&& ) = delete;

		~ShaderModule();

		VkShaderModule getShaderModule() const;

	private:
		// Temp -> Later pre-compile here
		std::vector<char> readSPIRVShaderFile( const Path& _filename );
		void createShaderModule( std::vector<char> _code );

		VkShaderModule m_ShaderModule;
		VkDevice& m_Device;
	};

	inline VkShaderModule Engine::ShaderModule::getShaderModule() const
	{
		return m_ShaderModule;
	}

} // end namespace Engine