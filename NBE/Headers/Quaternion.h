#pragma once
#include "Matrix.h"
#include "Vector3.h"
#include <cmath>
#define MyEpsilon 0.0001f
namespace NBE
{
    template <typename Real>
    class Quaternion
    {
    public:
        
        Real b,c,d;//i,j,k            //x,y,z
        Real a;//a real num			  //w

        inline explicit Quaternion():a(0),b(0),c(0),d(0)
        {}
        explicit Quaternion(Real _a,Real _b,Real _c,Real _d):a(_a),b(_b),c(_c),d(_d)
        {}
        explicit Quaternion(Real* pReal4):a(pReal4[0]),b(pReal4[1]),c(pReal4[2]),d(pReal4[3])
        {}
        explicit Quaternion(Real angleRadian,vec3<Real> axis)
        {
            Real tempAngle = angleRadian/2;
            a = cos(tempAngle);
            Real temp = sin(tempAngle);
            b = axis[0] * temp;
            c = axis[1] * temp;
            d = axis[2] * temp;
        }
        explicit Quaternion(Matrix3<Real>& r3x3)
        {
            //steal from OGRE's quaternion, this is awesome...

            // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
            // article "Quaternion Calculus and Fast Animation".

            Real fTrace = r3x3[0][0]+r3x3[1][1]+r3x3[2][2];
            Real fRoot;

            if ( fTrace > 0.0 )
            {
                // |w| > 1/2, may as well choose w > 1/2
                fRoot = sqrt(fTrace + 1.0f);  // 2w
                a = 0.5f*fRoot;
                fRoot = 0.5f/fRoot;  // 1/(4w)
                b = (r3x3[2][1]-r3x3[1][2])*fRoot;
                c = (r3x3[0][2]-r3x3[2][0])*fRoot;
                d = (r3x3[1][0]-r3x3[0][1])*fRoot;
            }
            else
            {
                // |w| <= 1/2
                static size_t s_iNext[3] = { 1, 2, 0 };
                size_t i = 0;
                if ( r3x3[1][1] > r3x3[0][0] )
                    i = 1;
                if ( r3x3[2][2] > r3x3[i][i] )
                    i = 2;
                size_t j = s_iNext[i];
                size_t k = s_iNext[j];

                fRoot = sqrt(r3x3[i][i]-r3x3[j][j]-r3x3[k][k] + 1.0f);
                Real* apkQuat[3] = { &a, &b, &c };
                *apkQuat[i] = 0.5f*fRoot;
                fRoot = 0.5f/fRoot;
                a = (r3x3[k][j]-r3x3[j][k])*fRoot;
                *apkQuat[j] = (r3x3[j][i]+r3x3[i][j])*fRoot;
                *apkQuat[k] = (r3x3[k][i]+r3x3[i][k])*fRoot;
            }
        }

        void normalize()
        {
            Real len = a * a + b * b + c * c + d * d;
            Real factor = 1.0f / sqrt(len);
            *this = *this * factor;
        }
        
        bool equals(const Quaternion& rhs, const Real tolerance = MyEpsilon) const
        {
            Real fCos = Dot(rhs);
            Real halfAngle = acos(fCos);
            return (abs(halfAngle) <= tolerance);
        }

        Real Dot(const Quaternion& r) const
        {
             return a*r.a+b*r.b+c*r.c+d*r.d;
        }

        Real angleRadian()
        {
            Real fSqrLength = b*b+c*c+d*d;
            if(fSqrLength > 0)
            {
                return 2 * acos(a);
            }
            else
            {
                return 0;
            }
        }

        vec3<Real> axis()
        {
            Real fSqrLength = b*b+c*c+d*d;
            if(fSqrLength > 0)
            {
                vec3<Real> temp(b,c,d);
                return temp / sin(acos(a)) ;
            }
            else
            {
                return vec3<Real>(1,0,0);//nomatter what axis is
            }
            
        }

        Matrix3<Real> toMat3()
        {
            //http://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
            //return Matrix3<Real>(a*a+b*b-c*c-d*d, 2*b*c-2*a*d, 2*b*d+2*a*c,
            //					 2*b*c+2*a*d, a*a-b*b+c*c-d*d, 2*c*d-2*a*b,
            //					 2*b*d-2*a*c, 2*c*d+2*a*b, a*a-b*b-c*c+d*d);
            
            return Matrix3<Real>( 1 - 2 * ( c*c + d*d ), 2 * ( b*c - d*a ), 2 * ( b*d + c*a ),
                                  2 * ( b*c + d*a ), 1 - 2 * ( b*b + d*d ), 2 * ( c*d - b*a ),
                                  2 * ( b*d - c*a ),2 * ( c*d + b*a ),  1 - 2 * ( b*b + c*c ) );
        }

        //operator
        Quaternion operator* (const Quaternion& r) const
        {
            return Quaternion(a*r.a - b*r.b - c*r.c - d*r.d,
                              a*r.b + b*r.a + c*r.d - d*r.c,
                              a*r.c - b*r.d + c*r.a + d*r.b,
                              a*r.d + b*r.c - c*r.b + d*r.a);
        }

        Quaternion operator* (Real Scalar) const
        {
            return Quaternion(Scalar*a,Scalar*b,Scalar*c,Scalar*d);
        }

    };

    typedef Quaternion<float> Quaternionf;
}