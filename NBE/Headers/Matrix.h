#pragma once
#include "NBE.h"
#include "Vector3.h"
#include "Vector4.h"
//#include "NBE.h"
//#include <memory.h>

namespace NBE
{
	template< typename Real >
	class Matrix3
	{
	private:
		Real m_matrix[3][3];
	public:
		inline Matrix3()
		{
			memset(m_matrix,0,sizeof(m_matrix));
		}
		inline explicit Matrix3(const Real array[3][3])
		{
			memcpy(m_matrix,array,sizeof(m_matrix));
		}
		inline Matrix3(Matrix3& m)
		{
			memcpy(m_matrix,m.m_matrix,sizeof(m_matrix));
		}

		Matrix3(vec3<Real>& v1,vec3<Real>& v2,vec3<Real>& v3)
		{
			m_matrix[0][0] = v1[0];
			m_matrix[0][1] = v1[1];
			m_matrix[0][2] = v1[2];
			m_matrix[1][0] = v2[0];
			m_matrix[1][1] = v2[1];
			m_matrix[1][2] = v2[2];
			m_matrix[2][0] = v3[0];
			m_matrix[2][1] = v3[1];
			m_matrix[2][2] = v3[2];
		}

		Matrix3(Real ar00, Real ar01, Real ar02,
			Real ar10, Real ar11, Real ar12,
			Real ar20, Real ar21, Real ar22)
		{
			m_matrix[0][0] = ar00;
			m_matrix[0][1] = ar01;
			m_matrix[0][2] = ar02;
			m_matrix[1][0] = ar10;
			m_matrix[1][1] = ar11;
			m_matrix[1][2] = ar12;
			m_matrix[2][0] = ar20;
			m_matrix[2][1] = ar21;
			m_matrix[2][2] = ar22;
		}

// 		Real operator[] (const int index)
// 		{
// 			return ((Real *)m_matrix)[index];
// 		}

		//Real& operator[] (const int index)
		//{
		//    myassert( 0 <= index && index < 9 );
		//    return ((Real *)m_matrix)[index];
		//}
		Real* operator[] (const int index)
		{
			myassert( 0 <= index && index < 3 );
			return &(((Real*)m_matrix)[index*3]);
		}

		Matrix3 operator+ (const Matrix3& other)const
		{
			Matrix3 result(this->m_matrix);
			result += other;
			return result;
		}

		Matrix3& operator+= (const Matrix3& other)
		{
			for (size_t i = 0; i < 3; ++i)
			{
				for (size_t j = 0; j < 3; ++j)
				{
					m_matrix[i][j] += other.m_matrix[i][j];
				}
			}
			return *this;
		}

		Matrix3 operator* (const Matrix3& other) const
		{
			Matrix3 temp;
			for (size_t i = 0; i < 3; ++i)
			{
				for (size_t j = 0; j < 3; ++j)
				{
					temp.m_matrix[i][j] = 
						m_matrix[i][0]*other.m_matrix[0][j] +
						m_matrix[i][1]*other.m_matrix[1][j] +
						m_matrix[i][2]*other.m_matrix[2][j];
				}
			}

			return temp;
		}

		Matrix3& operator= (const Matrix3& other)
		{
			memcpy(m_matrix,other.m_matrix,sizeof(m_matrix));
			return *this;
		}

		friend vec3<Real> operator* (const Matrix3& other, const vec3<Real>& vec)
		{
			vec3<Real> temp;
			for (size_t i = 0; i < 3; ++i)
			{
				temp[i] =
					other.m_matrix[i][0] * vec[0]+ 
					other.m_matrix[i][1] * vec[1]+ 
					other.m_matrix[i][2] * vec[2]; 
			}
			return temp;

		}//  matrix * vec  this is the 'transformation matrix'

		Matrix3 operator* (Real scalar)const
		{
			Matrix3 temp;
			for (size_t i = 0; i < 3; ++i)
			{
				for (size_t j = 0; j < 3; ++j)
				{
					temp.m_matrix[i][j] = m_matrix[i][j] * scalar;
				}
			}

			return temp;
		}

		friend Matrix3 operator* (Real scalar, Matrix3& other)
		{
			Matrix3 temp;
			for (size_t i = 0; i < 3; ++i)
			{
				for (size_t j = 0; j < 3; ++j)
				{
					temp.m_matrix[i][j] = other.m_matrix[i][j] * scalar;
				}
			}

			return temp;
		}

		static Matrix3 fromEulerAngles( Real pitch, Real yaw, Real roll )
		{
			//http://en.wikipedia.org/wiki/File:Rollpitchyawplain.png
			Real fCos, fSin;

			fCos = cos(yaw);
			fSin = sin(yaw);
			Matrix3<Real> kYMat(
				fCos,(Real)0.0,fSin,
				(Real)0.0,(Real)1.0,(Real)0.0,
				-fSin,(Real)0.0,fCos);

			fCos = cos(pitch);
			fSin = sin(pitch);
			Matrix3<Real> kXMat(
				(Real)1.0,(Real)0.0,(Real)0.0,
				(Real)0.0,fCos,-fSin,
				(Real)0.0,fSin,fCos);

			fCos = cos(roll);
			fSin = sin(roll);
			Matrix3<Real> kZMat(
				fCos,-fSin,(Real)0.0,
				fSin,fCos,(Real)0.0,
				(Real)0.0,(Real)0.0,(Real)1.0);

			//*this = kYMat*(kXMat*kZMat);
			return (kYMat*(kXMat*kZMat));
		}

		static Matrix3 Identity()
		{
			return Matrix3(1,0,0,0,1,0,0,0,1);
		}

		Matrix3 transpose()
		{
			std::swap(m_matrix[0][1],m_matrix[1][0]);
			std::swap(m_matrix[0][2],m_matrix[2][0]); 
			std::swap(m_matrix[1][2],m_matrix[2][1]);
	 
			return *this;
		}

		float determinant()
		{
			 float a = m_matrix[0][0]*( m_matrix[1][1]*m_matrix[2][2] - m_matrix[2][1]*m_matrix[1][2] );
			 float b = m_matrix[0][1]*( m_matrix[1][0]*m_matrix[2][2] - m_matrix[2][0]*m_matrix[1][2] );
			 float c = m_matrix[0][2]*( m_matrix[1][0]*m_matrix[2][1] - m_matrix[2][0]*m_matrix[1][1] );
			 return a - b + c;
		}

		Matrix3 inverse()
		{
			float det = determinant();
			Matrix3 result;
			result.m_matrix[0][0] =  (m_matrix[1][1]*m_matrix[2][2]-m_matrix[2][1]*m_matrix[1][2])/det;
			result.m_matrix[1][0] = -(m_matrix[0][1]*m_matrix[2][2]-m_matrix[0][2]*m_matrix[2][1])/det;
			result.m_matrix[2][0] =  (m_matrix[0][1]*m_matrix[1][2]-m_matrix[0][2]*m_matrix[1][1])/det;
			result.m_matrix[0][1] = -(m_matrix[1][0]*m_matrix[2][2]-m_matrix[1][2]*m_matrix[2][0])/det;
			result.m_matrix[1][1] =  (m_matrix[0][0]*m_matrix[2][2]-m_matrix[0][2]*m_matrix[2][0])/det;
			result.m_matrix[2][1] = -(m_matrix[0][0]*m_matrix[1][2]-m_matrix[1][0]*m_matrix[0][2])/det;
			result.m_matrix[0][2] =  (m_matrix[1][0]*m_matrix[2][1]-m_matrix[2][0]*m_matrix[1][1])/det;
			result.m_matrix[1][2] = -(m_matrix[0][0]*m_matrix[2][1]-m_matrix[2][0]*m_matrix[0][1])/det;
			result.m_matrix[2][2] =  (m_matrix[0][0]*m_matrix[1][1]-m_matrix[1][0]*m_matrix[0][1])/det;
			return result;
		}

	};

	typedef Matrix3<float> Matrix3f;

	template< typename Real >
	class Matrix4
	{
	private:
		Real m_matrix[4][4];
	public:
		inline Matrix4()
		{
			memset(m_matrix,0,sizeof(m_matrix));
		}
		inline explicit Matrix4(const Real array[4][4])
		{
			memcpy(m_matrix,array,sizeof(m_matrix));
		}

		//inline explicit Matrix4(const Real array[16])
		//{
		//	memcpy(m_matrix,array,sizeof(m_matrix));
		//}

		//inline Matrix4(Matrix4& m)
		//{
		//	memcpy(m_matrix,m.m_matrix,sizeof(Real)*16);
		//}

		inline Matrix4(const Matrix4& m)
		{
			memcpy(m_matrix,m.m_matrix,sizeof(m_matrix));
		}

		Matrix4(Matrix3<Real>& mat3)
		{
			m_matrix[0][0] = mat3[0][0];
			m_matrix[0][1] = mat3[0][1];
			m_matrix[0][2] = mat3[0][2];
			m_matrix[0][3] = 0;
			m_matrix[1][0] = mat3[1][0];
			m_matrix[1][1] = mat3[1][1];
			m_matrix[1][2] = mat3[1][2];
			m_matrix[1][3] = 0;
			m_matrix[2][0] = mat3[2][0];
			m_matrix[2][1] = mat3[2][1];
			m_matrix[2][2] = mat3[2][2];
			m_matrix[2][3] = 0;
			m_matrix[3][0] = 0;
			m_matrix[3][1] = 0;
			m_matrix[3][2] = 0;
			m_matrix[3][3] = 1;
		}

		Matrix4(Real ar00, Real ar01, Real ar02, Real ar03,
			Real ar10, Real ar11, Real ar12, Real ar13,
			Real ar20, Real ar21, Real ar22, Real ar23,
			Real ar30, Real ar31, Real ar32, Real ar33)
		{
			m_matrix[0][0] = ar00;
			m_matrix[0][1] = ar01;
			m_matrix[0][2] = ar02;
			m_matrix[0][3] = ar03;
			m_matrix[1][0] = ar10;
			m_matrix[1][1] = ar11;
			m_matrix[1][2] = ar12;
			m_matrix[1][3] = ar13;
			m_matrix[2][0] = ar20;
			m_matrix[2][1] = ar21;
			m_matrix[2][2] = ar22;
			m_matrix[2][3] = ar23;
			m_matrix[3][0] = ar30;
			m_matrix[3][1] = ar31;
			m_matrix[3][2] = ar32;
			m_matrix[3][3] = ar33;
		}

		Matrix4 operator* (const Matrix4& other) const
		{
			Matrix4 temp;
			for (size_t i = 0; i < 4; ++i)
			{
				for (size_t j = 0; j < 4; ++j)
				{
					temp.m_matrix[i][j] = 
						m_matrix[i][0]*other.m_matrix[0][j] +
						m_matrix[i][1]*other.m_matrix[1][j] +
						m_matrix[i][2]*other.m_matrix[2][j] +
						m_matrix[i][3]*other.m_matrix[3][j];
				}
			}

			return temp;
		}

		Matrix4 operator* (Real scalar)
		{
			Matrix4 temp;
			for (size_t i = 0; i < 4; ++i)
			{
				for (size_t j = 0; j < 4; ++j)
				{
					 temp.m_matrix[i][j] = m_matrix[i][j] * scalar;
				}
			}

			return temp;
		}

		Matrix4& operator= (const Matrix4& other)
		{
			memcpy(m_matrix,other.m_matrix,sizeof(m_matrix));
			return *this;
		}

		friend vec3<Real> operator* (const Matrix4& lh, const vec3<Real>& vec)
		{
			vec4<Real> temp;
			for (size_t i = 0; i < 4; ++i)
			{
				temp[i] =
					lh.m_matrix[i][0] * vec[0]+ 
					lh.m_matrix[i][1] * vec[1]+ 
					lh.m_matrix[i][2] * vec[2]+
					lh.m_matrix[i][3] * 1; //12,13,14 are translations, not 3,7,11
			}
			return vec3<Real>(&temp[0]);

		}

// 		Real operator[] (const int index)
// 		{
// 			 return ((Real *)m_matrix)[index];
// 		}
		Real& operator[] (const int index)
		{
			return ((Real *)m_matrix)[index];
		}

		Matrix3<Real> getMat3x3()
		{
			return 
			Matrix3<Real>(
			m_matrix[0][0], 
			m_matrix[0][1], 
			m_matrix[0][2],
			m_matrix[1][0], 
			m_matrix[1][1], 
			m_matrix[1][2],
			m_matrix[2][0], 
			m_matrix[2][1], 
			m_matrix[2][2]);
			 
		}

		Real* data()
		{
			return (Real*)m_matrix;
		}

		Matrix4 inverse()
		{
			//steal from OGRE engine
			Real m00 = m_matrix[0][0], m01 = m_matrix[0][1], m02 = m_matrix[0][2], m03 = m_matrix[0][3];
			Real m10 = m_matrix[1][0], m11 = m_matrix[1][1], m12 = m_matrix[1][2], m13 = m_matrix[1][3];
			Real m20 = m_matrix[2][0], m21 = m_matrix[2][1], m22 = m_matrix[2][2], m23 = m_matrix[2][3];
			Real m30 = m_matrix[3][0], m31 = m_matrix[3][1], m32 = m_matrix[3][2], m33 = m_matrix[3][3];

			Real v0 = m20 * m31 - m21 * m30;
			Real v1 = m20 * m32 - m22 * m30;
			Real v2 = m20 * m33 - m23 * m30;
			Real v3 = m21 * m32 - m22 * m31;
			Real v4 = m21 * m33 - m23 * m31;
			Real v5 = m22 * m33 - m23 * m32;

			Real t00 = + (v5 * m11 - v4 * m12 + v3 * m13);
			Real t10 = - (v5 * m10 - v2 * m12 + v1 * m13);
			Real t20 = + (v4 * m10 - v2 * m11 + v0 * m13);
			Real t30 = - (v3 * m10 - v1 * m11 + v0 * m12);

			Real invDet = 1 / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

			Real d00 = t00 * invDet;
			Real d10 = t10 * invDet;
			Real d20 = t20 * invDet;
			Real d30 = t30 * invDet;

			Real d01 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
			Real d11 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
			Real d21 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
			Real d31 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

			v0 = m10 * m31 - m11 * m30;
			v1 = m10 * m32 - m12 * m30;
			v2 = m10 * m33 - m13 * m30;
			v3 = m11 * m32 - m12 * m31;
			v4 = m11 * m33 - m13 * m31;
			v5 = m12 * m33 - m13 * m32;

			Real d02 = + (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
			Real d12 = - (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
			Real d22 = + (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
			Real d32 = - (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

			v0 = m21 * m10 - m20 * m11;
			v1 = m22 * m10 - m20 * m12;
			v2 = m23 * m10 - m20 * m13;
			v3 = m22 * m11 - m21 * m12;
			v4 = m23 * m11 - m21 * m13;
			v5 = m23 * m12 - m22 * m13;

			Real d03 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
			Real d13 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
			Real d23 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
			Real d33 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;
			 return	 Matrix4(
				d00, d01, d02, d03,
				d10, d11, d12, d13,
				d20, d21, d22, d23,
				d30, d31, d32, d33);
	 
		}

		Matrix4 transpose()
		{
			std::swap(m_matrix[0][1],m_matrix[1][0]);
			std::swap(m_matrix[0][2],m_matrix[2][0]);
			std::swap(m_matrix[0][3],m_matrix[3][0]);
			std::swap(m_matrix[1][2],m_matrix[2][1]);
			std::swap(m_matrix[1][3],m_matrix[3][1]);
			std::swap(m_matrix[2][3],m_matrix[3][2]);
			return *this;
		}
		Matrix4 fromEulerAngles( Real pitch, Real yaw, Real roll )
		{
			Real fCos, fSin;

			fCos = cos(yaw);
			fSin = sin(yaw);
			Matrix4<Real> kYMat(
				fCos,(Real)0.0,fSin, (Real)0.0,
				(Real)0.0,(Real)1.0,(Real)0.0, (Real)0.0,
				-fSin,(Real)0.0,fCos, (Real)0.0,
				(Real)0.0, (Real)0.0, (Real)0.0, (Real)1);

			fCos = cos(pitch);
			fSin = sin(pitch);
			Matrix4<Real> kXMat(
				(Real)1.0,(Real)0.0,(Real)0.0, (Real)0.0,
				(Real)0.0,fCos,-fSin, (Real)0.0,
				(Real)0.0,fSin,fCos, (Real)0.0,
				(Real)0.0, (Real)0.0, (Real)0.0, (Real)1);

			fCos = cos(roll);
			fSin = sin(roll);
			Matrix3<Real> kZMat(
				fCos,-fSin,(Real)0.0, (Real)0.0,
				fSin,fCos,(Real)0.0, (Real)0.0,
				(Real)0.0,(Real)0.0,(Real)1.0, (Real)0.0,
				(Real)0.0, (Real)0.0, (Real)0.0, (Real)1);

			*this = kYMat*(kXMat*kZMat);
			return *this;
		}

		static Matrix4 Identity()
		{
			return Matrix4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
		}
	};

	typedef Matrix4<float> Matrix4f;
}