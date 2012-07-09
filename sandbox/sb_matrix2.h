#ifndef SB_MATRIX2_H_INCLUDED
#define SB_MATRIX2_H_INCLUDED


#include "sb_vector2.h"

/// swap
#include <algorithm>

namespace Sandbox {


    /// 2D vector
    struct Matrix2f
    {
        float matrix[4];

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
        Matrix2f(float m00, float m01, float m10, float m11)
        {
                matrix[0 * 2 + 0] = m00; matrix[0 * 2 + 1] = m01;
                matrix[1 * 2 + 0] = m10; matrix[1 * 2 + 1] = m11;
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
       
        Matrix2f &inverse() {
            float det = matrix[0*2+0]*matrix[1*2+1] - matrix[0*2+1]*matrix[1*2+0];
            std::swap(matrix[0*2+0],matrix[1*2+1]);
            float idet = 1.0f / det;
            matrix[0*2+0]*=idet;
            matrix[0*2+1]*=-idet;
            matrix[1*2+0]*=-idet;
            matrix[1*2+1]*=idet;
            return *this;
        }

   
        Matrix2f &operator =(const Matrix2f &copy) { 
            matrix[0*2+0]=copy.matrix[0*2+0];
            matrix[0*2+1]=copy.matrix[0*2+1];
            matrix[1*2+0]=copy.matrix[1*2+0];
            matrix[1*2+1]=copy.matrix[1*2+1];
            return *this; 
        }

      
        Matrix2f operator *(const Matrix2f &m) const { 
            return Matrix2f( 
                matrix[0*2+0]*m.matrix[0*2+0]+matrix[1*2+0]*m.matrix[0*2+1],
                matrix[0*2+1]*m.matrix[0*2+0]+matrix[1*2+1]*m.matrix[0*2+1],
                matrix[0*2+0]*m.matrix[1*2+0]+matrix[1*2+0]*m.matrix[1*2+1],
                matrix[0*2+1]*m.matrix[1*2+0]+matrix[1*2+1]*m.matrix[1*2+1]
            );
        }

        Matrix2f& operator *=(const Matrix2f &m)  { 
            return *this = *this * m;
        }

        Matrix2f operator +(const Matrix2f &m) const { 
            return Matrix2f( 
                matrix[0*2+0]+m.matrix[0*2+0],
                matrix[0*2+1]+m.matrix[0*2+1],
                matrix[1*2+0]+m.matrix[1*2+0],
                matrix[1*2+1]+m.matrix[1*2+1] 
            );
        }
        
        Matrix2f operator -(const Matrix2f &m) const { 
            return Matrix2f( 
                matrix[0*2+0]-m.matrix[0*2+0],
                matrix[0*2+1]-m.matrix[0*2+1],
                matrix[1*2+0]-m.matrix[1*2+0],
                matrix[1*2+1]-m.matrix[1*2+1] 
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
                    m.matrix[0*2+0]*v.x + m.matrix[0*2+1]*v.y,
                    m.matrix[1*2+0]*v.x + m.matrix[1*2+1]*v.y);
    }

    inline Vector2f operator * (const Matrix2f& m, const Vector2f& v)
    {
            return Vector2f(
                    m.matrix[0*2+0]*v.x + m.matrix[1*2+0]*v.y,
                    m.matrix[0*2+1]*v.x + m.matrix[1*2+1]*v.y);
    }



}

#endif /*SB_MATRIX2_H_INCLUDED*/
