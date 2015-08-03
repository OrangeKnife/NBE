#pragma once
namespace NBE
{
    template< typename Real >
    struct vec4
    {
        Real x, y, z, w;
        explicit vec4( Real _x = 0, Real _y = 0, Real _z = 0, Real _w = 0) : x( _x ), y( _y ), z( _z ), w( _w ) 
        {}
        explicit vec4( float* pReal4)
        {
            memcpy_s(&x,4*sizeof(float),pReal4,4*sizeof(float));
        }
        explicit vec4( int* pReal4):x((Real)*pReal4),y((Real)pReal4[1]),z((Real)pReal4[2]),w((Real)pReal4[3])
        {}
        Real& operator[](int i)
        {
            myassert( 0 <= i && i<4);
            return (&x)[i];
        }
    };
    typedef vec4< float > vec4f;
}