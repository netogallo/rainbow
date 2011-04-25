#ifndef VEC3_H_
#define VEC3_H_

namespace Rainbow
{
	/// Structure for storing a three-dimensional vector.

	/// Copyright 2010 Bifrost Games. All rights reserved.
	/// \author Tommy Nguyen
	template<typename T>
	struct __Vec3
	{
		T x, y, z;

		__Vec3() : x(0), y(0), z(0) { }
		__Vec3(const T x, const T y, const T z) : x(x), y(y), z(z) { }

		/// Determine whether the vector is zero.
		inline bool is_zero() const
		{
			return (this->x == 0) && (this->y == 0) && (this->z == 0);
		}

		/// Zero the vector.
		void zero()
		{
			this->x = 0;
			this->y = 0;
			this->z = 0;
		}

		__Vec3<T>& operator+=(const __Vec3<T> &v)
		{
			this->x += v.x;
			this->y += v.y;
			this->z += v.z;
			return *this;
		}

		__Vec3<T>& operator-=(const __Vec3<T> &v)
		{
			this->x -= v.x;
			this->y -= v.y;
			this->z -= v.z;
			return *this;
		}

		__Vec3<T>& operator*=(const T &f)
		{
			this->x *= f;
			this->y *= f;
			this->z *= f;
			return *this;
		}
	};
}

typedef Rainbow::__Vec3<float> Vec3f;

#endif