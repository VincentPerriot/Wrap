#pragma once

#include "../Utils/Common.h"
#include "Vector3.h"


namespace Maths {
	class LinAlg
	{
	public:
		static constexpr f32 Dot( const Vector3& _a, const Vector3& _b );
		static constexpr Vector3 Cross( const Vector3& _left, const Vector3& _right );
	};

	constexpr f32 Maths::LinAlg::Dot( const Vector3& _a, const Vector3& _b )
	{
		return _a.x * _b.x + _a.y * _b.y + _a.z * _b.z;
	}

	constexpr Maths::Vector3 Maths::LinAlg::Cross( const Vector3& _left, const Vector3& _right )
	{
		return Vector3{
			.x = _left.y * _right.z - _left.z * _right.y,
			.y = _left.z * _right.x - _left.x * _right.z,
			.z = _left.x * _right.y - _left.y * _right.x
		};
	}

} // end namespace Maths

