#include "ShaderModule.h"

namespace Engine {
	//----------------------------------------------------------------------------------
	ShaderModule::ShaderModule( const Path& _filename, VkDevice& _device )
		: m_Device( _device )
	{
		auto code = readSPIRVShaderFile( _filename );
		createShaderModule( code );
	}

	//----------------------------------------------------------------------------------
	ShaderModule::~ShaderModule()
	{
		vkDestroyShaderModule( m_Device, m_ShaderModule, nullptr );
	}

	//----------------------------------------------------------------------------------
	std::vector<char> ShaderModule::readSPIRVShaderFile( const Path& _filename )
	{
		std::ifstream file( _filename, std::ios::ate | std::ios::binary );

		if ( !file.is_open() ) {
			throw std::runtime_error( "failed to open file!" );
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer( fileSize );

		file.seekg( 0 );
		file.read( buffer.data(), fileSize );

		file.close();

		return buffer;
	}

	//----------------------------------------------------------------------------------
	void ShaderModule::createShaderModule( std::vector<char> _code )
	{
		VkShaderModuleCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.codeSize = _code.size(),
			.pCode = reinterpret_cast<const uint32_t*>( _code.data() )
		};

		VK_ASSERT( vkCreateShaderModule( m_Device, &createInfo, nullptr, &m_ShaderModule ) );
	}

} // end namespace Engine
