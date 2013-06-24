#pragma once
#ifndef IDOCK_UTILITY_HPP
#define IDOCK_UTILITY_HPP

#include <array>
#include "vec3.hpp"
#include "qtn4.hpp"
using namespace std;

/// Returns the flattened 1D index of a 2D index (i, j) where j is the lowest dimension.
inline size_t mr(const size_t i, const size_t j)
{
	return (j*(j+1)>>1) + i;
}

/// Returns the flattened 1D index of a 2D index (i, j) where either i or j is the lowest dimension.
inline size_t mp(const size_t i, const size_t j)
{
	return i <= j ? mr(i, j) : mr(j, i);
}

/// Returns the square norm of current quaternion.
inline float norm_sqr(const qtn4& q)
{
	return q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3];
}

/// Returns the norm of current quaternion.
inline float norm(const qtn4& q)
{
	return sqrt(norm_sqr(q));
}

/// Returns true if the current quaternion is normalized.
inline bool is_normalized(const qtn4& q)
{
	return norm_sqr(q) - 1.0f < 1e-5f;
}

/// Returns a normalized quaternion of current quaternion.
inline qtn4 normalize(const qtn4& q)
{
	const float norm_inv = 1.0f / norm(q);
	return qtn4(q[0] * norm_inv, q[1] * norm_inv, q[2] * norm_inv, q[3] * norm_inv);
}

/// Constructs a quaternion by a normalized axis and a rotation angle.
inline qtn4 vec4_to_qtn4(const vec3& axis, const float angle)
{
//	assert(axis.normalized());
	const float s = sin(angle * 0.5f);
	return qtn4
	(
		cos(angle * 0.5f),
		s * axis[0],
		s * axis[1],
		s * axis[2]
	);
}

/// Constructs a quaternion by a rotation vector.
inline qtn4 vec3_to_qtn4(const vec3& rotation)
{
	if (rotation.zero())
	{
		return qtn4(1, 0, 0, 0);
	}
	else
	{
		const float angle = rotation.norm();
		const vec3 axis = (1.0f / angle) * rotation;
		return vec4_to_qtn4(axis, angle);
	}
}

/// Returns the product of two quaternions.
inline qtn4 qtn4_mul_qtn4(const qtn4& q1, const qtn4& q2)
{
    return qtn4
	(
		q1[0] * q2[0] - q1[1] * q2[1] - q1[2] * q2[2] - q1[3] * q2[3],
		q1[0] * q2[1] + q1[1] * q2[0] + q1[2] * q2[3] - q1[3] * q2[2],
		q1[0] * q2[2] - q1[1] * q2[3] + q1[2] * q2[0] + q1[3] * q2[1],
		q1[0] * q2[3] + q1[1] * q2[2] - q1[2] * q2[1] + q1[3] * q2[0]
	);
}

/// Transforms the current quaternion into a 3x3 transformation matrix, e.g. quaternion(1, 0, 0, 0) => identity matrix.
inline array<float, 9> qtn4_to_mat3(const qtn4& q)
{
//	assert(is_normalized());
	const float aa = q[0]*q[0];
	const float ab = q[0]*q[1];
	const float ac = q[0]*q[2];
	const float ad = q[0]*q[3];
	const float bb = q[1]*q[1];
	const float bc = q[1]*q[2];
	const float bd = q[1]*q[3];
	const float cc = q[2]*q[2];
	const float cd = q[2]*q[3];
	const float dd = q[3]*q[3];

	// http://www.boost.org/doc/libs/1_46_1/libs/math/quaternion/TQE.pdf
	// http://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
	return
	{
		aa+bb-cc-dd, 2*(-ad+bc), 2*(ac+bd),
		2*(ad+bc), aa-bb+cc-dd, 2*(-ab+cd),
		2*(-ac+bd), 2*(ab+cd), aa-bb-cc+dd
	};
}

/// Transforms a vector by a 3x3 matrix.
inline vec3 mat3_mul_vec3(const array<float, 9>& m, const vec3& v)
{
	return
	{
		m[0] * v[0] + m[1] * v[1] + m[2] * v[2],
		m[3] * v[0] + m[4] * v[1] + m[5] * v[2],
		m[6] * v[0] + m[7] * v[1] + m[8] * v[2]
	};
}

/// Transforms a vector by a 3x3 matrix.
inline array<float, 3> mat3_mul_vec3(const array<float, 9>& m, const array<float, 3>& v)
{
	return
	{
		m[0] * v[0] + m[1] * v[1] + m[2] * v[2],
		m[3] * v[0] + m[4] * v[1] + m[5] * v[2],
		m[6] * v[0] + m[7] * v[1] + m[8] * v[2]
	};
}

#endif