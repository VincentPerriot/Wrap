#pragma once

#include "../Utils/Common.h"

namespace Maths {
	struct Vector3 final
	{
		f32 x{}, y{}, z{};

		constexpr Vector3 operator+( const Vector3& _rhs ) const;
		constexpr Vector3 operator-( const Vector3& _rhs ) const;

		inline Vector3& Normalize();
		inline f32 Length() const;
	};

	constexpr Maths::Vector3 Maths::Vector3::operator+( const Vector3& _rhs ) const
	{
		return Vector3{ .x = x + _rhs.x, .y = y + _rhs.y, .z = z + _rhs.z };
	}

	constexpr Maths::Vector3 Maths::Vector3::operator-( const Vector3& _rhs ) const
	{
		return Vector3{ .x = x - _rhs.x, .y = y - _rhs.y, .z = z - _rhs.z };
	}

	inline f32 Maths::Vector3::Length() const
	{
		return std::sqrt( x * x + y * y + z * z );
	}

	inline Maths::Vector3& Maths::Vector3::Normalize()
	{
		if ( auto length{ Length() }; length != 0.0f )
		{
			x = x / length;
			y = y / length;
			z = z / length;
		}

		return *this;
	}
} // end namespace Maths
