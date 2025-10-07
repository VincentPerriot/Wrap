#pragma once

#include "../Utils/Common.h"

namespace Maths {
	struct Vector3 final
	{
		f32 x{}, y{}, z{};

		constexpr Vector3 Dot( const Vector3& _rhs );
		constexpr Vector3 Cross( const Vector3& _rhs );
		constexpr Vector3& Normalize();
	};

	constexpr Maths::Vector3 Maths::Vector3::Dot( const Vector3& _rhs )
	{

	}

	constexpr Maths::Vector3 Maths::Vector3::Cross( const Vector3& _rhs )
	{

	}

	constexpr Maths::Vector3& Maths::Vector3::Normalize()
	{
		return *this;
	}
} // end namespace Maths
