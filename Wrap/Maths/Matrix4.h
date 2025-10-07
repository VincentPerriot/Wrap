#pragma once

#include "Vector4.h"
#include "Vector3.h"

namespace Maths {

	struct Matrix4 final
	{
		Vector4 c1{}, c2{}, c3{}, c4{};

		constexpr Matrix4 Identity();
		constexpr Matrix4 Model( const Vector3& _pos, const Vector3& _rot, const Vector3& _scale );
		constexpr Matrix4 View( const Vector3& _eye, const Vector3& _target, const Vector3 _up );
		constexpr Matrix4 Projection( f32 _fov, f32 _aspect, f32 _near, f32 _far);
	};

	constexpr Maths::Matrix4 Matrix4::Identity()
	{
		return Matrix4{
			.c1 = Vector4{ 1, 0, 0, 0  },
			.c2 = Vector4{ 0, 1, 0, 0 },
			.c3 = Vector4{ 0, 0, 1, 0 },
			.c4 = Vector4{ 0, 0, 0, 1 }
		};
	}

	constexpr Maths::Matrix4 Matrix4::Model( const Vector3& _pos, const Vector3& _rot, const Vector3& _scale )
	{

	}

	constexpr Maths::Matrix4 Matrix4::View( const Vector3& _eye, const Vector3& _target, const Vector3 _up )
	{

	}

	constexpr Maths::Matrix4 Matrix4::Projection( f32 _fov, f32 _aspect, f32 _near, f32 _far )
	{

	}

} // end namespace Maths

