#include "UnitConvert.h"

//------------------------------------------------------------------------------------
f32 Utils::UnitConvert::RadiansToDegrees( f32 _rad )
{
	return _rad * ( 180.0f / PI );
}

//------------------------------------------------------------------------------------
f32 Utils::UnitConvert::DegreesToRadians( f32 _deg )
{
	return _deg * ( PI / 180.0f );
}

