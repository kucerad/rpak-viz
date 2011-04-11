//-----------------------------------------------------------------------------
//  [GSY] Utilities - Simple 4D vector class
//  27/02/2008
//-----------------------------------------------------------------------------
//  Controls: 
//-----------------------------------------------------------------------------


#ifndef __VECTOR4_H__
#define __VECTOR4_H__

#include "Vector3.h"

class Vector4
{
	public:
		inline static const Vector4& zero()  { static Vector4 v(0.0f, 0.0f, 0.0f, 0.0f); return v; }

	public:
		Vector4()
		{
			x = y = z = w = 0;
		}
		
		Vector4(float _x, float _y, float _z, float _w = 1.0f)
		{
			x = _x;
			y = _y;
			z = _z;
			w = _w;
		}
	
		Vector4(const Vector3& copy)
		{
			x = copy.x;
			y = copy.y;
			z = copy.z;
			w = 1.0f;
		}

		Vector4(const Vector4& copy)
		{
			x = copy.x;
			y = copy.y;
			z = copy.z;
			w = copy.w;
		}

		Vector4(const flint &copy){
			x = float(copy.ch[0]);
			y = float(copy.ch[1]);
			z = float(copy.ch[2]);
			w = float(copy.ch[3]);
		}
		inline void setFromV3(const Vector3 &vec){
			x = vec.x;
			y = vec.y;
			z = vec.z;
		}
		inline operator const float*() const	{ return &x;	}
		inline operator float*()				{ return &x;	}
	
//		inline float x() const					{ return x;	}
//		inline float y() const					{ return y;	}
//		inline float z() const					{ return z;	}
//		inline float w() const					{ return w;	}
		inline Vector3	xyz()					{ return Vector3(x, y, z);	}

		inline Vector4& operator = (const Vector4& copy)
		{
			x = copy.x;
			y = copy.y;
			z = copy.z;
			w = copy.w;
			return *this;
		}
	
		inline bool operator == (const Vector4& vec)
		{
			return (x == vec.x) && (y == vec.y) && (z == vec.z) && (w == vec.w);
		}
		
		inline bool operator != (const Vector3& vec)
		{
			return !(operator==(vec));
		}
		
		inline void set(float _x, float _y, float _z, float _w)
		{
			x = _x;
			y = _y;
			z = _z;
			w = _w;
		}
		inline Vector4 operator + (const Vector4& vec) const
		{
			return Vector4(x + vec.x, y + vec.y, z + vec.z, w + vec.w);
		}

		inline Vector4 operator - (const Vector4& vec) const
		{
			return Vector4(x - vec.x, y - vec.y, z - vec.z, w - vec.w);
		}


		inline Vector4 operator / (float scalar)
		{
			return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
		}
		inline Vector4 operator * (float scalar) const
		{
			return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
		}
		inline Vector4 operator + (float scalar) const
		{
			return Vector4(x + scalar, y + scalar, z + scalar, w + scalar);
		}
		inline Vector4 operator - (float scalar) const
		{
			return Vector4(x - scalar, y - scalar, z - scalar, w - scalar);
		}
		inline Vector4 operator - () const
		{
			return Vector4(-x, -y, -z, -w);
		}
		
	public:
		union
		{
			struct{float x, y, z, w;};
			struct{float r, s, t, u;};
			struct{float r, g, b, a;};
			float data[4];
		};
};
typedef Vector4 v4;
typedef Vector4 Color;
#endif __VECTOR4_H__
