#pragma once

#include "../Utils/Common.h"

namespace Maths {
	struct Vector2 final
	{
		f32 x{}, y{};

		constexpr Vector2 operator+( const Vector2& _rhs );
	};

	constexpr Maths::Vector2 Maths::Vector2::operator+( const Vector2& _rhs )
	{
		return Vector2{ .x = x + _rhs.x, .y = y + _rhs.y };
	}

} // end namespace Maths

