#pragma once

#include "Common.h"

namespace Utils 
{
	class UnitConvert
	{
	public:
		static constexpr f32 RadiansToDegrees( f32 _rad );
		static constexpr f32 DegreesToRadians( f32 _deg );
	};

	//------------------------------------------------------------------------------------
	constexpr f32 Utils::UnitConvert::RadiansToDegrees( f32 _rad )
	{
		return _rad * ( 180.0f / PI );
	}

	//------------------------------------------------------------------------------------
	constexpr f32 Utils::UnitConvert::DegreesToRadians( f32 _deg )
	{
		return _deg * ( PI / 180.0f );
	}
} // end namespace Utils
