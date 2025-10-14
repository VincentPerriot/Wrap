#pragma once

#include "Vector4.h"
#include "Vector3.h"
#include "../Utils/UnitConvert.h"
#include "LinAlg.h"

namespace Maths {

	enum class AngleUnit {
		DEGREES,
		RADIANS
	};

	struct Matrix4 final
	{
		Vector4 c1{}, c2{}, c3{}, c4{};

		constexpr static Matrix4 Identity();
		inline static Matrix4 DefaultModelMatrix();
		inline static Matrix4 Model( const Vector3& _pos, const Vector3& _rot, const Vector3& _scale, AngleUnit _angleUnit = AngleUnit::DEGREES );
		inline static Matrix4 View( const Vector3& _eye, const Vector3& _target, const Vector3 _up );
		inline static Matrix4 Projection( f32 _fov, f32 _aspect, f32 _near, f32 _far, AngleUnit _angleUnit = AngleUnit::DEGREES );
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

	Maths::Matrix4 Matrix4::DefaultModelMatrix()
	{
		return Matrix4::Model( Maths::Vector3{ 0.0f, 0.0f, 0.f }, Maths::Vector3{ 0.0f, 0.0f, 0.0f }, Maths::Vector3{ 1.0f, 1.0f, 1.0f } );
	}

	inline Maths::Matrix4 Matrix4::Model( const Vector3& _pos, const Vector3& _rot, const Vector3& _scale, AngleUnit _angleUnit /*= Maths::AngleUnit::DEGREES*/ )
	{
		Vector3 r = _rot;

		if ( _angleUnit != AngleUnit::RADIANS )
		{
			r.x = Utils::UnitConvert::DegreesToRadians( r.x );
			r.y = Utils::UnitConvert::DegreesToRadians( r.y );
			r.z = Utils::UnitConvert::DegreesToRadians( r.z );
		}

		const f32 cx = std::cos( r.x );
		const f32 sx = std::sin( r.x );
		const f32 cy = std::cos( r.y );
		const f32 sy = std::sin( r.y );
		const f32 cz = std::cos( r.z );
		const f32 sz = std::sin( r.z );

		const f32 m00 = cy * cz;
		const f32 m01 = -cy * sz;
		const f32 m02 = sy;
		const f32 m10 = cz * sx * sy + cx * sz;
		const f32 m11 = cx * cz - sx * sy * sz;
		const f32 m12 = -sx * cy;
		const f32 m20 = -sy * cz;
		const f32 m21 = sy * sz;
		const f32 m22 = cy;

		return Matrix4{
			.c1 = Vector4{ m00 * _scale.x, m01 * _scale.x, m02 * _scale.x, 0.0f },
			.c2 = Vector4{ m10 * _scale.y, m11 * _scale.y, m12 * _scale.y, 0.0f },
			.c3 = Vector4{ m20 * _scale.z, m21 * _scale.z, m22 * _scale.z, 0.0f },
			.c4 = Vector4{ _pos.x, _pos.y, _pos.z, 1.0f }
		};
	}

	inline Maths::Matrix4 Matrix4::View( const Vector3& _eye, const Vector3& _target, const Vector3 _upWorld )
	{
		using namespace Maths;

		const Vector3 fwd = Vector3{ _target - _eye }.Normalize();

		if ( LinAlg::Cross( fwd, _upWorld ).Length() < 1e-6f )
		{
			return Identity();
		}

		const Vector3 right = LinAlg::Cross( fwd, _upWorld ).Normalize();
		const Vector3 camUp = LinAlg::Cross( right, fwd );

		return Matrix4{
			.c1 = Vector4{ right.x, right.y, right.z ,0.0f},
			.c2 = Vector4{ camUp.x, camUp.y, camUp.z ,0.0f},
			.c3 = Vector4{ -fwd.x, -fwd.y, -fwd.z ,0.0f},
			.c4 = Vector4{ -LinAlg::Dot( right, _eye ), -LinAlg::Dot( camUp, _eye ), -LinAlg::Dot( fwd, _eye ) ,1.0f}
		};
	}

	inline Maths::Matrix4 Matrix4::Projection( f32 _fov, f32 _aspect, f32 _near, f32 _far, AngleUnit _angleUnit /*= Maths::AngleUnit::DEGREES*/ )
	{
		if ( _angleUnit != AngleUnit::RADIANS )
		{
			_fov = Utils::UnitConvert::DegreesToRadians( _fov );
		}
		const f32 thf = std::tan( _fov / 2.0f );
		const f32 a = 1.0f / ( thf * _aspect );
		const f32 b = 1.0f / thf;
		const f32 c = _far / ( _far - _near );
		const f32 d = -( _far * _near ) / ( _far - _near );

		return Matrix4{
			.c1 = Vector4{ a, 0.0f, 0.0f,  0.0f },
			.c2 = Vector4{ 0.0f, b, 0.0f, 0.0f },
			.c3 = Vector4{ 0.0f, 0.0f, c, 1.0f },
			.c4 = Vector4{ 0.0f, 0.0f, d, 0.0f }
		};
	}

} // end namespace Maths

