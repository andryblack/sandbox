#ifndef SB_MATRIX2_H_INCLUDED
#define SB_MATRIX2_H_INCLUDED


#include "sb_vector2.h"

/// swap
#include <algorithm>

namespace Sandbox {


    /// 2D matrix
    union Matrix2f
    {
        float matrix[4];
        struct {
            float a; float b;
            float c; float d;
        } comp;
        Matrix2f() { }

        Matrix2f(const Matrix2f &copy)
        {
            for (int i=0; i<4; i++)
                matrix[i] = copy.matrix[i];
        }

        ///  Constructs a 2x2 matrix (copied from 4 floats)
        explicit Matrix2f(const float init_matrix[4])
        {
            for (int i=0; i<4; i++)
               matrix[i] = init_matrix[i];
        }

        /// Constructs a 2x2 matrix (copied from specified values)
        Matrix2f(float a, float b, float c, float d)
        {
            comp.a = a; comp.b = b;
            comp.c = c; comp.d = d;
        }

        static Matrix2f null() { return Matrix2f(0.0f,0.0f,0.0f,0.0f); }

        static Matrix2f identity() { return Matrix2f(1.0f,0.0f,0.0f,1.0f); }


        /// Multiply this matrix and a specified matrix.
        Matrix2f &multiply(const Matrix2f &m) {
            return *this = *this * m;
        }

        /// Add this matrix and a specified matrix.
        Matrix2f &add(const Matrix2f &m) {
            return *this = *this + m;
        }

        /// Subtract this matrix and a specified matrix.
        Matrix2f &subtract(const Matrix2f &m) {
            return *this = *this - m;
        }

        float get_determinant() const {
            return comp.a*comp.d - comp.b*comp.c;
        }

        Matrix2f &inverse(float det) {
            std::swap(comp.a,comp.d);
            float idet = 1.0f / det;
            comp.a*=idet;
            comp.b*=-idet;
            comp.c*=-idet;
            comp.d*=idet;
            return *this;
        }

        Matrix2f &inverse() {
            float det = get_determinant();
            return inverse(det);
        }
   
        Matrix2f &operator =(const Matrix2f &copy) { 
            matrix[0*2+0]=copy.matrix[0*2+0];
            matrix[0*2+1]=copy.matrix[0*2+1];
            matrix[1*2+0]=copy.matrix[1*2+0];
            matrix[1*2+1]=copy.matrix[1*2+1];
            return *this; 
        }


        Matrix2f operator *(const Matrix2f &m) const {
            /*
             [a1,b1] x [a2,b2] = [a1*a2+c1*b2, b1*a2+d1*b2],
             [c1,d1]   [c2,d2]   [a1*c2+c1*d2, b1*c2+d1*d2]
             */
            return Matrix2f( 
                            comp.a*m.comp.a+comp.c*m.comp.b,
                            comp.b*m.comp.a+comp.d*m.comp.b,
                            comp.a*m.comp.c+comp.c*m.comp.d,
                            comp.b*m.comp.c+comp.d*m.comp.d
            );
        }
        
        Matrix2f operator *(float k) const {
            return Matrix2f(
                            comp.a*k,
                            comp.b*k,
                            comp.c*k,
                            comp.d*k
                            );
        }


        Matrix2f& operator *=(const Matrix2f &m)  { 
            return *this = *this * m;
        }
        
        void scale(float sx,float sy) {
            /*
               [a, b] x [ sx, 0 ] = [a*sx+c*0 ,b*sx+d*0 ] = [a*sx,b*sx]
               [c, d]   [ 0,  sy]   [a*0 +c*sy,b*0 +d*sy]   [c*sy,d*sy]
             */
            comp.a *= sx;
            comp.b *= sx;
            comp.c *= sy;
            comp.d *= sy;
        }
        void rotate(float sine,float cosine) {
            // m*=Matrix2f(c,s,-s,c);
            /*
             [a, b] x [ cos,  sin ] = [ a*cos+c*sin, b*cos+d*sin ]
             [c, d]   [-sin,  cos ]   [-a*sin+c*cos,-b*sin+d*cos]
             */
            // a
            float tmp = comp.a*cosine + comp.c*sine;
            comp.c = -comp.a*sine + comp.c*cosine;
            comp.a = tmp;
            // b
            tmp = comp.b*cosine + comp.d*sine;
            comp.d = -comp.b*sine + comp.d*cosine;
            comp.b = tmp;
        }
        void rotate_scale(float sine,float cosine,float sx, float sy) {
            /*
             [a, b] x [ cos,  sin ] = [ a*cos+c*sin, b*cos+d*sin ] x [ sx, 0 ] = [ (a*cos+c*sin)*sx, (b*cos+d*sin)*sx]
             [c, d]   [-sin,  cos ]   [-a*sin+c*cos,-b*sin+d*cos]    [ 0, sy ]   [(-a*sin+c*cos)*sy,(-b*sin+d*cos)*sy]
             */
            // a
            float tmp = comp.a*cosine + comp.c*sine;
            comp.c = (-comp.a*sine + comp.c*cosine) * sy;
            comp.a = tmp * sx;
            // b
            tmp = comp.b*cosine + comp.d*sine;
            comp.d = (-comp.b*sine + comp.d*cosine) * sy;
            comp.b = tmp * sx;
        }

        Matrix2f operator +(const Matrix2f &m) const { 
            return Matrix2f( 
                            comp.a+m.comp.a,
                            comp.b+m.comp.b,
                            comp.c+m.comp.c,
                            comp.d+m.comp.d
            );
        }
        
        Matrix2f operator -(const Matrix2f &m) const { 
            return Matrix2f( 
                            comp.a-m.comp.a,
                            comp.b-m.comp.b,
                            comp.c-m.comp.c,
                            comp.d-m.comp.d
            );
        }
        
        bool operator==(const Matrix2f &other) const
        {
            for (int i=0; i<4; i++)
                if (matrix[i] != other.matrix[i]) return false;
            return true;
        }

        bool operator!=(const Matrix2f &other) const { return !((*this) == other); }

    };




    inline Vector2f operator * (const Vector2f& v, const Matrix2f& m)
    {
            return Vector2f(
                    m.comp.a*v.x + m.comp.b*v.y,
                    m.comp.c*v.x + m.comp.d*v.y);
    }

    inline Vector2f operator * (const Matrix2f& m, const Vector2f& v)
    {
            return Vector2f(
                    m.comp.a*v.x + m.comp.c*v.y,
                    m.comp.b*v.x + m.comp.d*v.y);
    }



}

#endif /*SB_MATRIX2_H_INCLUDED*/
