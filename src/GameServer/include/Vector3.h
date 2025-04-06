#ifndef __Vector3_H__
#define __Vector3_H__

#include <math.h>

class Vector3
{
	public:
		union
		{
			struct
			{
				float x, y, z;
			};
			float val[3];
		};
	
	public:
		inline Vector3()
		{
		}
	
		inline Vector3(const float fX, const float fY, const float fZ)
			: x(fX), y(fY), z(fZ)
		{
		}

		inline explicit Vector3(const float afCoordinate[3])
			: x(afCoordinate[0]),
			  y(afCoordinate[1]),
			  z(afCoordinate[2])
		{
		}
		
		inline explicit Vector3(float* const r)
			: x(r[0]), y(r[1]), z(r[2])
		{
		}

		inline explicit Vector3(const float scaler)
			: x(scaler)
			, y(scaler)
			, z(scaler)
		{
		}

        inline void Normal()
		{
			float module;
			module = x * x + y * y + z * z;
			module = (float)sqrt((double)module);
            
            x /= module;
            y /= module;
            z /= module;
		}

        inline float Length() const
        {
            return (float)sqrt( LengthSquare() );
        }

        inline float LengthSquare() const
        {
			return x * x + y * y + z * z;
        }

		inline Vector3 Normal(const Vector3& rkVector)
		{
			float module;

			module = rkVector.x * rkVector.x + rkVector.y * rkVector.y + rkVector.z * rkVector.z;
			module = (float)sqrt((double)module);
			return (rkVector / module);
		}

		inline Vector3 operator = (const Vector3& rkVector)
		{
			x = rkVector.x;
			y = rkVector.y;
			z = rkVector.z;

			return *this;
		}

		inline Vector3& operator = (const float fScaler)
		{
			x = fScaler;
			y = fScaler;
			z = fScaler;

			return *this;
		}

		inline bool operator == (const Vector3& rkVector) const
		{
			return (x == rkVector.x && y == rkVector.y && z == rkVector.z);
		}

		inline bool operator <= (const Vector3& rkVector) const
		{
			return (x <= rkVector.x && y <= rkVector.y && z <= rkVector.z);
		}

		inline bool operator >= (const Vector3& rkVector) const
		{
			return (x >= rkVector.x && y >= rkVector.y && z >= rkVector.z);
		}

		inline bool operator != (const Vector3& rkVector) const
		{
			return (x != rkVector.x || y != rkVector.y || z != rkVector.z);
		}

		inline Vector3 operator + (const Vector3& rkVector) const
		{
			Vector3 kSum;

			kSum.x = x + rkVector.x;
			kSum.y = y + rkVector.y;
			kSum.z = z + rkVector.z;

			return kSum;
		}

		inline Vector3 operator - (const Vector3& rkVector) const
		{
			Vector3 kDiff;

			kDiff.x = x - rkVector.x;
			kDiff.y = y - rkVector.y;
			kDiff.z = z - rkVector.z;

			return kDiff;
		}

		inline Vector3 operator * (const float fScalar) const
		{
			Vector3 kProd;

			kProd.x = fScalar * x;
			kProd.y = fScalar * y;
			kProd.z = fScalar * z;
			
			return kProd;
		}

		inline Vector3 operator * (const Vector3& rhs) const
		{
			Vector3 kProd;

			kProd.x = rhs.x * x;
			kProd.y = rhs.y * y;
			kProd.z = rhs.z * z;

			return kProd;
		}

		inline Vector3 operator / (const float fScalar) const
		{
			Vector3 kProd;

			kProd.x = kProd.x / fScalar;
			kProd.y = kProd.y / fScalar;
			kProd.z = kProd.z / fScalar;

			return kProd;
		}

		inline Vector3& operator *= (const float fScalar)
		{
			x *= fScalar;
			y *= fScalar;
			z *= fScalar;
			
			return *this;
		}
};

#endif