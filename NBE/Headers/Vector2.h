#pragma once
#include "MyAssert.h"
namespace NBE
{
    template< typename Real >
    struct vec2
    {
        Real x, y;
        explicit vec2( Real _x = 0, Real _y = 0) : x( _x ), y( _y ) {}
        Real& operator[](int index)
        {
            myassert(0<= index && index < 2);
            return (&x)[index];
        }

        vec2 operator+( const vec2& other) const
        {
            vec2 result(*this);
            result += other;
            return result;
        }

        vec2 operator-( const vec2& other) const
        {
            vec2 result(*this);
            result -= other;
            return result;
        }

        vec2 operator-(void) 
        {
            vec2 result(*this);

            result.x = -x;
            result.y = -y;

            return result;
        }

        vec2& operator+=( const vec2& other)
        {
            x += other.x;
            y += other.y;
            
            return *this;
        }

        vec2& operator-=( const vec2& other)
        {
            x -= other.x;
            y -= other.y;
            
            return *this;
        }

        vec2& operator*=( Real scalar)
        {
            x *= scalar;
            y *= scalar;
            
            return *this;
        }

        vec2 operator*( Real scalar)const
        {
            vec2 result(*this);
            result *= scalar;
            return result;
        }
    };
    typedef vec2< float > vec2f;
}