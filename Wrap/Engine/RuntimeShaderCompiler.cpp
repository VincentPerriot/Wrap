#include "RuntimeShaderCompiler.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>

#include <vulkan/vulkan.h>

#include <shaderc/shaderc.h>
#include "glslang/Include/glslang_c_interface.h"
#include "glslang/Public/resource_limits_c.h"

namespace Engine {
	//--------------------------------------------------------------------
	bool RuntimeShaderCompiler::compile( std::filesystem::path _source, std::filesystem::path _dest )
	{
		std::string source = _source.string();
		std::string dest = _dest.string();

		std::string sourceShader = readShaderFile( source );
		std::vector<uint8_t> spirv;

		if ( _compile( vkShaderStageFromFile( source ), sourceShader.c_str(), &spirv, glslang_default_resource() ) )
		{
			saveSPRIVBin( dest, spirv.data(), spirv.size() );
			return true;
		}

		return false;
	}

	//--------------------------------------------------------------------
	std::string RuntimeShaderCompiler::readShaderFile( std::string_view _filename )
	{
		std::ifstream filestream( _filename.data() );
		std::string code;

		char BOM[3];
		filestream.read( BOM, 3 );

		if ( !( BOM[0] == char( 0xEF ) && BOM[1] == char( 0xBB ) && BOM[2] == char( 0xBF ) ) )
		{
			filestream.seekg( 0 );
		}

		if ( filestream.is_open() )
		{
			code.assign( std::istreambuf_iterator<char>( filestream ), std::istreambuf_iterator<char>() );
			filestream.close();
		}
		else
		{
			std::cerr << "Unable to open: " << _filename << std::endl;
			return std::string{};
		}

		// No include system available
		while ( code.find( "#include " ) != code.npos )
		{
			const auto pos = code.find( "#include " );
			const auto start = code.find( '<' );
			const auto end = code.find( '>' );

			if ( start == code.npos || end == code.npos || end <= start )
			{
				std::cerr << "Error loading import on shader" << _filename << std::endl;
				return std::string{};
			}
			const std::string inc_name = code.substr( start + 1, end - 1 );
			const std::string inc_shader = readShaderFile( inc_name );

			code.replace( pos, end - pos + 1, inc_shader );
		}

		return code;
	}

	//--------------------------------------------------------------------
	void RuntimeShaderCompiler::saveSPRIVBin( std::string_view _filename, const uint8_t* _code, size_t _size )
	{
		std::ofstream out( _filename.data(), std::ios::binary );

		if ( !out )
		{
			std::cerr << "Error writing to " << _filename << " Verify file exists";
			return;
		}

		out.write( reinterpret_cast<const char*>( _code ), _size );
	}

	//--------------------------------------------------------------------
	VkShaderStageFlagBits RuntimeShaderCompiler::vkShaderStageFromFile( std::string_view _filename )
	{
		auto ext = _filename.substr( _filename.rfind( '.' ) );
		ext.remove_suffix( ext.size() - ext.find_last_not_of( " \r\n\t" ) - 1 );

		if ( ext == ".vert" )
		{
			return VK_SHADER_STAGE_VERTEX_BIT;
		}
		if ( ext == ".frag" )
		{
			return VK_SHADER_STAGE_FRAGMENT_BIT;
		}
		if ( ext == ".geom" )
		{
			return VK_SHADER_STAGE_GEOMETRY_BIT;
		}
		if ( ext == ".comp" )
		{
			return VK_SHADER_STAGE_COMPUTE_BIT;
		}
		if ( ext == ".tesc" )
		{
			return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		}
		if ( ext == ".tese" )
		{
			return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		}

		assert( false );
		return VK_SHADER_STAGE_VERTEX_BIT;
	}

	//--------------------------------------------------------------------
	glslang_stage_t RuntimeShaderCompiler::getGlslLangStage( VkShaderStageFlagBits _stage )
	{
		switch ( _stage )
		{
		case VK_SHADER_STAGE_VERTEX_BIT:
			return GLSLANG_STAGE_VERTEX;
		case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
			return GLSLANG_STAGE_TESSCONTROL;
		case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
			return GLSLANG_STAGE_TESSEVALUATION;
		case VK_SHADER_STAGE_GEOMETRY_BIT:
			return GLSLANG_STAGE_GEOMETRY;
		case VK_SHADER_STAGE_FRAGMENT_BIT:
			return GLSLANG_STAGE_FRAGMENT;
		case VK_SHADER_STAGE_COMPUTE_BIT:
			return GLSLANG_STAGE_COMPUTE;
		default:
			break;
		}

		return GLSLANG_STAGE_VERTEX;
	}

	//--------------------------------------------------------------------
	bool RuntimeShaderCompiler::_compile( VkShaderStageFlagBits _stage, const char* _code, std::vector<uint8_t>* _outSPIRV, const glslang_resource_t* _glslLangResource )
	{
		if ( !glslang_initialize_process() ) {
			std::cerr << "Failed to initialize glslang process" << std::endl;
			return false;
		}

		const glslang_input_t input = {
				.language = GLSLANG_SOURCE_GLSL,
				.stage = getGlslLangStage( _stage ),
				.client = GLSLANG_CLIENT_VULKAN,
				.client_version = GLSLANG_TARGET_VULKAN_1_3,
				.target_language = GLSLANG_TARGET_SPV,
				.target_language_version = GLSLANG_TARGET_SPV_1_6,
				.code = _code,
				.default_version = 450,
				.default_profile = GLSLANG_NO_PROFILE,
				.force_default_version_and_profile = false,
				.forward_compatible = false,
				.messages = GLSLANG_MSG_DEFAULT_BIT,
				.resource = _glslLangResource
		};

		glslang_shader_t* shader = glslang_shader_create( &input );

		if ( !glslang_shader_preprocess( shader, &input ) )
		{
			std::cerr << "Shader Process Failed" << std::endl;
			std::cerr << glslang_shader_get_info_log( shader ) << std::endl;
			std::cerr << glslang_shader_get_info_debug_log( shader ) << std::endl;
			return false;
		}
		if ( !glslang_shader_parse( shader, &input ) )
		{
			std::cerr << "Shader Parse Failed" << std::endl;
			std::cerr << glslang_shader_get_info_log( shader ) << std::endl;
			std::cerr << glslang_shader_get_info_debug_log( shader ) << std::endl;
			return false;
		}

		glslang_program_t* program = glslang_program_create();
		glslang_program_add_shader( program, shader );

		if ( !glslang_program_link( program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT ) )
		{
			std::cerr << "Program Linking Failed" << std::endl;
			std::cerr << glslang_program_get_info_log( program ) << std::endl;
			std::cerr << glslang_program_get_info_debug_log( program ) << std::endl;
			return false;
		}

		glslang_spv_options_t options = {
			.generate_debug_info = true,
			.strip_debug_info = false,
			.disable_optimizer = false,
			.optimize_size = true,
			.disassemble = false,
			.validate = true,
			.emit_nonsemantic_shader_debug_info = false,
			.emit_nonsemantic_shader_debug_source = false
		};

		glslang_program_SPIRV_generate_with_options( program, input.stage, &options );

		if ( glslang_program_SPIRV_get_messages( program ) )
		{
			std::cout << glslang_program_SPIRV_get_messages( program ) << std::endl;
		}
		else
		{
			std::cout << "GLSL Shader successfully compiled, generating SPIRV file." << std::endl;
		}

		const uint8_t* spirv = reinterpret_cast<const uint8_t*>( glslang_program_SPIRV_get_ptr( program ) );
		const size_t size = glslang_program_SPIRV_get_size( program ) * sizeof( uint32_t );

		*_outSPIRV = std::vector( spirv, spirv + size );

		glslang_program_delete( program );
		glslang_shader_delete( shader );

		glslang_finalize_process();

		return true;
	}
} // end namespace Engine
