#pragma once
#include <filesystem>
#include <span>
#include "glslang/Include/glslang_c_interface.h"
#include "vulkan/vulkan_core.h"

namespace Engine {
	class RuntimeShaderCompiler
	{
	public:
		static bool compile( std::filesystem::path _source, std::filesystem::path _dest );

	private:
		static std::string readShaderFile( std::string_view _filename );
		static void saveSPRIVBin( std::string_view _filename, const uint8_t* _code, size_t size );

		static VkShaderStageFlagBits vkShaderStageFromFile( std::string_view _filename );
		static glslang_stage_t getGlslLangStage( VkShaderStageFlagBits _stage );

		static bool _compile( VkShaderStageFlagBits stage, const char* code, std::vector<uint8_t>* outSPIRV, const glslang_resource_t* glslLangResource );
	};
} // end namespace Engine