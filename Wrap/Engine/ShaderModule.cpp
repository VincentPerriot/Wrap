#include "ShaderModule.h"

namespace Engine {
	//----------------------------------------------------------------------------------
	ShaderModule::ShaderModule( const Path& _filename, VkDevice& _device )
		: m_Device( _device )
	{
		auto code = readShaderFile( _filename );
		createShaderModule( code );
	}

	//----------------------------------------------------------------------------------
	ShaderModule::~ShaderModule()
	{
		vkDestroyShaderModule( m_Device, m_ShaderModule, nullptr );
	}

	//----------------------------------------------------------------------------------
	std::vector<char> ShaderModule::readShaderFile( const Path& _filename )
	{
		// ate -> seek end of stream immediately after open
		std::ifstream stream( _filename, std::ios::binary | std::ios::ate );
		assert( stream );

		auto size = (size_t)stream.tellg();
		std::vector<char> buffer(size);

		stream.seekg( 0, std::ios::beg );
		assert( stream.read( buffer.data(), size ) );

		stream.close();
		return buffer;
	}

	//----------------------------------------------------------------------------------
	void ShaderModule::createShaderModule( std::vector<char> _code )
	{
		VkShaderModuleCreateInfo createInfo {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.codeSize = _code.size(),
			.pCode = reinterpret_cast<const u32*>( _code.data() )
		};

		VK_ASSERT( vkCreateShaderModule( m_Device, &createInfo, nullptr, &m_ShaderModule ) );
	}

} // end namespace Engine
