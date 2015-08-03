#pragma once
#include <math.h>
// PI/360
#define  PI_OVER_360 0.0087266462599716478846184538424431f
namespace NBE
{
    template< typename real >
    inline real clamp( real value, real min, real max )
    {
        return value < min ? min : value > max ? max : value;
    }

    template< typename T, typename AlphaT >
    inline T Lerp( const T& start, const T& end, AlphaT alpha )
    {
        return start + (end - start) * alpha;
    }


    template<typename real>
    class Plane
    {
    public:
        inline Plane(){}
        inline Plane(Plane& other)
        {
            n = other.n;
            d = other.d;
        }
        inline Plane(vec3<real> normal,real distance)
        {
            n = normal;
            d = distance;
        }
        inline void setPlane(vec3<real>& normal, real distance)
        {
            n = normal;
            d = distance; // -(normal * point); // inner product
        }

        inline void setPlane(vec3<real>& normal, vec3<real>& point)
        {
            n = normal;
            d = -(normal * point); // inner product
        }

        void Plane::set3Points( vec3f &v1,  vec3f &v2,  vec3f &v3) {
            vec3f aux1, aux2;
            aux1 = v1 - v2;
            aux2 = v3 - v2;
            n = aux2.crossProduct(aux1);
            n.normalize();
            vec3f point = v2;
            d = -(n * point);
        }

        inline real getDistance(vec3<real>& point)
        {
            return d+(n * point); // * is inner product
        }

        inline vec3<real> getNormal()
        {
            return n;
        }
    private:
        vec3<real> n;
        //vec3<real> point;
        real d;
    };


    inline bool SameSide(const vec3f &pos1, const vec3f &pos2, const vec3f &a, const vec3f &b)
    {
        //used to check if a point is inside a triangle
        vec3f ab = b - a;
        vec3f d1 = pos1 - a;
        vec3f d2 = pos2 - a;

        vec3f cross1 = ab.crossProduct(d1);

        vec3f cross2 = ab.crossProduct(d2);

        float fdot = cross1 * cross2;

        if(fdot >= 0)
            return true;
        else
            return false;
    }

    inline bool pointInTriangle(const vec3f &pos, const vec3f &posA, const vec3f &posB, const vec3f &posC)
    {
        //use to detect the sphere intersect with a triangle

        //if the pos is the same side with one vertex of the triangle ABC,
        //like pos and A are the same side of line BC
        return  SameSide(pos, posA, posB, posC) && 
                SameSide(pos, posB, posA, posC) && 
                SameSide(pos, posC, posA, posB) ;
            
         
    }

    inline bool sphereIntersectTriangle(
        vec3f& p1,
        vec3f& p2,
        vec3f& p3,
        vec3f& pos,
        float radius
        )
    {
        Plane<float> triPlane;
        triPlane.set3Points(p1,p2,p3);
        float distance = triPlane.getDistance(pos);
        if(distance > radius || distance < -radius )
            return false;
        //
        vec3f posInPlane;//center project point in plane
         posInPlane = pos +  triPlane.getNormal() * distance;
        
        float radiusInPlane  = sqrt(radius * radius - distance * distance);
        return (posInPlane.dis(p1) < radiusInPlane 
            || posInPlane.dis(p2) < radiusInPlane
            || posInPlane.dis(p3) < radiusInPlane
            || pointInTriangle(posInPlane,p1,p2,p3));
        
    }

    template<typename real>
    class Line
    {
        //vec3<real>& StartPoint(){return pos1;}
        //vec3<real>& EndPoint(){return pos2;}
    public:
        Line<real>(vec3<real> p1,vec3<real> p2):
          pos1(p1),pos2(p2)
          {}
        vec3<real> pos1;
        vec3<real> pos2;
    };
     

}