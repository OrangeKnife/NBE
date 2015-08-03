#pragma once
#include "MyAssert.h"
#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062f

namespace NBE
{
    template< typename Real > 
    struct vec3
    {
        Real x, y, z;
        explicit vec3(Real _x = 0, Real _y = 0, Real _z = 0): x(_x),y(_y),z(_z){}
        explicit vec3(int* _array):x((Real)_array[0]),y((Real)_array[1]),z((Real)_array[2]){}
        explicit vec3(float* _array):x((Real)_array[0]),y((Real)_array[1]),z((Real)_array[2]){}
        
        const Real dis(const vec3& other) const
        {
            vec3 minus(x,y,z);
            minus -= other;
            return sqrt(minus.x * minus.x + minus.y * minus.y + minus.z * minus.z);
        }

        const Real length()
        {
            return sqrt( x*x + y*y +z*z );
        }
        vec3 normalize()
        {
            float len = sqrt( x*x + y*y +z*z );
            if (len == 0)
            {
                return *this;
            }
            x /= len;
            y /= len;
            z /= len;
            return *this;
        }
        Real operator[](int i)const
        {
            myassert( 0 <= i && i<3);
            return (&x)[i];
        }

        Real& operator[](int i)
        {
            myassert( 0 <= i && i<3);
            return (&x)[i];
        }
        vec3 operator+( const vec3& other) const
        {
            vec3 result(*this);
            result += other;
            return result;
        }

        vec3 operator-( const vec3& other) const
        {
            vec3 result(*this);
            result -= other;
            return result;
        }

        vec3 operator-(void) 
        {
            vec3 result(*this);

            result.x = -x;
            result.y = -y;
            result.z = -z;

            return result;
        }

        vec3& operator+=( const vec3& other)
        {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        vec3& operator-=( const vec3& other)
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        vec3& operator*=( Real scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

        vec3 operator*( Real scalar)const
        {
            vec3 result(*this);
            result *= scalar;
            return result;
        }

        Real operator*( vec3& other)const
        {
            return x * other.x + y * other.y + z * other.z; // inner product
        }

        vec3 crossProduct(vec3 &other) {

            vec3 res;

            res.x = y * other.z - z * other.y;
            res.y = z * other.x - x * other.z;
            res.z = x * other.y - y * other.x;

            return res;
        }


        bool operator==( const vec3& other)
        {
            return (x == other.x && y == other.y && z == other.z);
        }

        bool operator!=( const vec3& other)
        {
            return (x != other.x || y != other.y || z != other.z);
        }

        friend vec3 operator* (const Real scalar, const vec3& other)
        {
            vec3 res;
            for (size_t i = 0; i < 3; ++i)
            {
                res[i] = scalar * other[i];			 
            }
            return res;

        }// Real * vec3

    };

    typedef vec3< float > vec3f;
}