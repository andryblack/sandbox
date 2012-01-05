#ifndef SB_MATRIX2_H_INCLUDED
#define SB_MATRIX2_H_INCLUDED


#include "sb_vector2.h"

namespace Sandbox {


    struct Matrix2f;

    struct Matrix3f;

    struct Matrix4f;

  
    /// \brief 2D matrix
    ///
    struct Matrix2f
    {
  
            /// \brief Constructs a 2x2 matrix (uninitialised)
            Matrix2f() { }

            /// \brief Constructs a 2x2 matrix (copied)
            Matrix2f(const Matrix2f &copy)
            {
                    for (int i=0; i<4; i++)
                            matrix[i] = copy.matrix[i];
            }

            /// \brief Constructs a 2x2 matrix (copied from a 3d matrix)
            explicit Matrix2f(const Matrix3f &copy);

            /// \brief Constructs a 2x2 matrix (copied from a 4d matrix)
            explicit Matrix2f(const Matrix4f &copy);

            /// \brief Constructs a 2x2 matrix (copied from 4 floats)
            explicit Matrix2f(const float *init_matrix)
            {
                    for (int i=0; i<4; i++)
                            matrix[i] = (float) init_matrix[i];
            }

            /// \brief Constructs a 2x2 matrix (copied from specified values)
            Matrix2f(float m00, float m01, float m10, float m11)
            {
                    matrix[0 * 2 + 0] = m00; matrix[0 * 2 + 1] = m01;
                    matrix[1 * 2 + 0] = m10; matrix[1 * 2 + 1] = m11;
            }



            static Matrix2f null();

            static Matrix2f identity();

            /// \brief Multiply 2 matrices
            ///
            /// This multiplies the matrix as follows: result = matrix1 * matrix2
            ///
            /// \param matrix_1 = First Matrix to multiply
            /// \param matrix_2 = Second Matrix to multiply
            /// \return The matrix
            static Matrix2f multiply(const Matrix2f &matrix_1, const Matrix2f &matrix_2);

            /// \brief Add 2 matrices
            ///
            /// This adds the matrix as follows: result = matrix1 + matrix2
            ///
            /// \param matrix_1 = First Matrix to add
            /// \param matrix_2 = Second Matrix to add
            /// \return The matrix
            static Matrix2f add(const Matrix2f &matrix_1, const Matrix2f &matrix_2);

            /// \brief Subtract 2 matrices
            ///
            /// This subtract the matrix as follows: result = matrix1 - matrix2
            ///
            /// \param matrix_1 = First Matrix to subtract
            /// \param matrix_2 = Second Matrix to subtract
            /// \return The matrix
            static Matrix2f subtract(const Matrix2f &matrix_1, const Matrix2f &matrix_2);

   
            float matrix[4];

  
            /// \brief Multiply this matrix and a specified matrix.
            ///
            /// This multiplies the matrix as follows: this = mult * this
            ///
            /// \param mult = Matrix to multiply
            ///
            /// \return reference to this object
            Matrix2f &multiply(const Matrix2f &mult);

            /// \brief Add this matrix and a specified matrix.
            ///
            /// This adds the matrix as follows: this = add_matrix - this
            ///
            /// \param add_matrix = Matrix to add
            ///
            /// \return reference to this object
            Matrix2f &add(const Matrix2f &add_matrix);

            /// \brief Subtract this matrix and a specified matrix.
            ///
            /// This subtracts the matrix as follows: this = subtract_matrix - this
            ///
            /// \param subtract_matrix = Matrix to subtract
            ///
            /// \return reference to this object
            Matrix2f &subtract(const Matrix2f &subtract_matrix);

  
   
            Matrix2f &inverse();

   
            /// \brief Copy assignment operator.
            Matrix2f &operator =(const Matrix2f &copy) { 
				for (size_t i=0;i<2*2;i++) matrix[i]=copy.matrix[i];
				return *this; 
			}

            /// \brief Copy assignment operator.
            Matrix2f &operator =(const Matrix4f &copy);

            /// \brief Copy assignment operator.
            Matrix2f &operator =(const Matrix3f &copy);

            /// \brief Multiplication operator.
            Matrix2f operator *(const Matrix2f &mult) const { Matrix2f result = *this; result.multiply(mult); return result; }

            /// \brief Multiplication operator.
            Matrix2f& operator *=(const Matrix2f &mult)  { multiply(mult); return *this; }

            /// \brief Addition operator.
            Matrix2f operator +(const Matrix2f &add_matrix) const { Matrix2f result = *this; result.add(add_matrix); return result; }

            /// \brief Subtract operator.
            Matrix2f operator -(const Matrix2f &subtract_matrix) const { Matrix2f result = *this; result.subtract(subtract_matrix); return result; }

            /// \brief Equality operator.
            bool operator==(const Matrix2f &other)
            {
                    for (int i=0; i<4; i++)
                            if (matrix[i] != other.matrix[i]) return false;
                    return true;
            }

            /// \brief Not-equal operator.
            bool operator!=(const Matrix2f &other) { return !((*this) == other); }

  
    private:
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



    /////////////////////////////////////////////////////////////////////////////
   

    inline Matrix2f Matrix2f::null()
    {
            Matrix2f m;
			for (size_t i=0;i<4;i++) m.matrix[i]=0.0f;
            return m;
    }

    inline Matrix2f Matrix2f::identity()
    {
            Matrix2f m = null();
            m.matrix[0] = 1;
            m.matrix[3] = 1;
            return m;
    }

    inline Matrix2f Matrix2f::multiply(const Matrix2f &matrix_1, const Matrix2f &matrix_2)
    {
            Matrix2f dest(matrix_2);
            dest.multiply(matrix_1);
            return dest;
    }

    inline Matrix2f Matrix2f::add(const Matrix2f &matrix_1, const Matrix2f &matrix_2)
    {
            Matrix2f dest(matrix_2);
            dest.add(matrix_1);
            return dest;
    }

    inline Matrix2f Matrix2f::subtract(const Matrix2f &matrix_1, const Matrix2f &matrix_2)
    {
            Matrix2f dest(matrix_2);
            dest.subtract(matrix_1);
            return dest;
    }

    


    /////////////////////////////////////////////////////////////////////////////
    // Matrix2f operations:

    inline Matrix2f &Matrix2f::multiply(const Matrix2f &mult)
    {
            Matrix2f result;
            for (int x=0; x<2; x++)
            {
                    for (int y=0; y<2; y++)
                    {
                            result.matrix[x+y*2] =
                                    matrix[0*2 + x]*mult.matrix[y*2 + 0] +
                                    matrix[1*2 + x]*mult.matrix[y*2 + 1];
                    }
            }
            *this = result;
            return *this;
    }

    inline Matrix2f &Matrix2f::add(const Matrix2f &add_matrix)
    {
            matrix[(0*2) + 0] += add_matrix.matrix[(0*2) + 0];
            matrix[(0*2) + 1] += add_matrix.matrix[(0*2) + 1];
            matrix[(1*2) + 0] += add_matrix.matrix[(1*2) + 0];
            matrix[(1*2) + 1] += add_matrix.matrix[(1*2) + 1];
            return *this;
    }

    inline Matrix2f &Matrix2f::subtract(const Matrix2f &subtract_matrix)
    {
            matrix[(0*2) + 0] = subtract_matrix.matrix[(0*2) + 0] - matrix[(0*2) + 0];
            matrix[(0*2) + 1] = subtract_matrix.matrix[(0*2) + 1] - matrix[(0*2) + 1];
            matrix[(1*2) + 0] = subtract_matrix.matrix[(1*2) + 0] - matrix[(1*2) + 0];
            matrix[(1*2) + 1] = subtract_matrix.matrix[(1*2) + 1] - matrix[(1*2) + 1];
            return *this;
    }

    inline Matrix2f &Matrix2f::inverse() 
    {
        float det = matrix[0*2+0]*matrix[1*2+1] - matrix[0*2+1]*matrix[1*2+0];
        std::swap(matrix[0*2+0],matrix[1*2+1]);
        float idet = 1.0f / det;
        matrix[0*2+0]*=idet;
        matrix[0*2+1]*=-idet;
        matrix[1*2+0]*=-idet;
        matrix[1*2+1]*=idet;
        return *this;
    }
  






}

#endif /*SB_MATRIX2_H_INCLUDED*/
