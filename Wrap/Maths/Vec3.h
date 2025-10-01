#pragma once

#include "../Utils/Common.h"

struct Vector3 final
{
	f32 x{}, y{}, z{};

	constexpr [[no_discard]] Vector3 operator+( const Vector3& _rhs ) const;
};

constexpr [[no_discard]] Vector3 Vector3::operator+( const Vector3& _rhs ) const
{
	return Vector3{ x + _rhs.x, y + _rhs.y, z + _rhs.z };
}
