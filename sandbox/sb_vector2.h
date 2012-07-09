#ifndef SB_VECTOR2_H_INCLUDED
#define SB_VECTOR2_H_INCLUDED

#include "sb_math.h"

namespace Sandbox {

    /// 2D vector
    struct Vector2f
    {
   
        float x;
        float y;

        Vector2f() : x(0.0f), y(0.0f) { }
        Vector2f(float p1, float p2 ) : x(p1), y(p2) { }
        Vector2f(const Vector2f &copy) : x(copy.x), y(copy.y) {}

        /// @return the length of the vector
        float length() const { return sqrt(x*x+y*y); }
        
        /// Normalizes this vector
        Vector2f& normalize() { 
            float len = length(); 
            if ( len!=0.0f ) {
                x/=len;
                y/=len;
            }
            return *this;
        }
      
        /// Return normalized vector with some dir
        Vector2f unit() const { Vector2f res(*this); res.normalize(); return res; }

        /// Return normal for this vector
        Vector2f normal() const { return Vector2f(-y,x); }

        /// Dot products this vector with an other vector.
        float dot(const Vector2f& v) const {return x*v.x + y*v.y;}

        /// Calculate the angle between this vector and an other vector.
        float angle(const Vector2f& v) const {
            return acos(dot(v)/(length()*v.length()));
        }
        
        // Calculate the distance between this vector and an other vector.
        float distance(const Vector2f& v) const {
            return Vector2f(x-v.x,y-v.y).length();
        }

        /// Rotate this vector.
        Vector2f &rotate(float a) {
            const float sin_angle = sin(a);
            const float cos_angle = cos(a);
            
            const float dest_x =  x * cos_angle - y * sin_angle;
            const float dest_y =  x * sin_angle + y * cos_angle;
            
            x = dest_x;
            y = dest_y;
            return *this;
        }
        
        /// Direction of vector
        float dir() const { return atan2(x, -y); }
        
        void operator += (const Vector2f& v) { x+= v.x; y+= v.y; }

        void operator += ( float value) { x+= value; y+= value; }

        Vector2f operator + (const Vector2f& v) const {return Vector2f(v.x + x, v.y + y);}
        
        Vector2f operator + (float value) const {return Vector2f(value + x, value + y);}

        void operator -= (const Vector2f& v) { x-= v.x; y-= v.y; }

        void operator -= ( float value) { x-= value; y-= value; }

        Vector2f operator - (const Vector2f& v) const {return Vector2f(x - v.x, y - v.y);}

        Vector2f operator - (float value) const {return Vector2f(x - value, y - value);}

        Vector2f operator - () const {return Vector2f(-x , -y);}

        void operator *= (const Vector2f& v) { x*= v.x; y*= v.y; }

        void operator *= ( float value) { x*= value; y*= value; }

        Vector2f operator * (const Vector2f& v) const {return Vector2f(v.x * x, v.y * y);}

        Vector2f operator * (float value) const {return Vector2f(value * x, value * y);}

        void operator /= (const Vector2f& v) { x/= v.x; y/= v.y; }

        void operator /= ( float value) { x/= value; y/= value; }

        Vector2f operator / (const Vector2f& v) const {return Vector2f(x / v.x, y / v.y);}

        Vector2f operator / (float value) const {return Vector2f(x / value, y / value);}

        Vector2f &operator = (const Vector2f& v) { x = v.x; y = v.y; return *this; }

        bool operator == (const Vector2f& v) const {return ((x == v.x) && (y == v.y));}

        bool operator != (const Vector2f& v) const {return ((x != v.x) || (y != v.y));}
    };
  
}


#endif /*SB_VECTOR2_H_INCLUDED*/
