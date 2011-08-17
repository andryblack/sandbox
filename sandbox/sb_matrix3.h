#ifndef SB_MATRIX3_H_INCLUDED
#define SB_MATRIX3_H_INCLUDED



#include "sb_matrix2.h"
#include "sb_vector3.h"

namespace Sandbox {

    struct Matrix2f;

    struct Matrix3f;

    struct Matrix4f;

	
    /// \brief 3D matrix
    ///
    struct Matrix3f
    {
  
            /// \brief Constructs a 3x3 matrix (uninitialised)
            Matrix3f() { }

            /// \brief Constructs a 3x3 matrix (copied)
            Matrix3f(const Matrix3f &copy)
            {
                    for (int i=0; i<9; i++)
                            matrix[i] = copy.matrix[i];
            }

            /// \brief Constructs a 3x3 matrix (copied from a 2d matrix)
            explicit Matrix3f(const Matrix2f &copy);

            /// \brief Constructs a 3x3 matrix (copied from a 4d matrix)
            explicit Matrix3f(const Matrix4f &copy);

            /// \brief Constructs a 3x3 matrix (copied from 9 floats)
            explicit Matrix3f(const float *init_matrix)
            {
                    for (int i=0; i<9; i++)
                            matrix[i] = (float) init_matrix[i];
            }

            /// \brief Constructs a 3x3 matrix (copied from specified values)
            Matrix3f(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22)
            {
                    matrix[0 * 3 + 0] = m00; matrix[0 * 3 + 1] = m01; matrix[0 * 3 + 2] = m02;
                    matrix[1 * 3 + 0] = m10; matrix[1 * 3 + 1] = m11; matrix[1 * 3 + 2] = m12;
                    matrix[2 * 3 + 0] = m20; matrix[2 * 3 + 1] = m21; matrix[2 * 3 + 2] = m22;
            }



            static Matrix3f null();

            static Matrix3f identity();

            /// \brief Create a rotation matrix
            ///
            /// Matrix is created in the Column-Major matrix format (opengl native)
            /// \param angle = Angle to rotate by
            /// \param x = Amount to rotate in the X axis
            /// \param y = Amount to rotate in the Y axis
            /// \param z = Amount to rotate in the Z axis
            /// \param normalize = true = Normalize x,y,z before creating rotation matrix
            /// \return The matrix (in column-major format)
            static Matrix3f rotate(float angle, float x, float y, float z, bool normalize = true);

            /// \brief Multiply 2 matrices
            ///
            /// This multiplies the matrix as follows: result = matrix1 * matrix2
            ///
            /// \param matrix_1 = First Matrix to multiply
            /// \param matrix_2 = Second Matrix to multiply
            /// \return The matrix
            static Matrix3f multiply(const Matrix3f &matrix_1, const Matrix3f &matrix_2);

            /// \brief Add 2 matrices
            ///
            /// This adds the matrix as follows: result = matrix1 + matrix2
            ///
            /// \param matrix_1 = First Matrix to add
            /// \param matrix_2 = Second Matrix to add
            /// \return The matrix
            static Matrix3f add(const Matrix3f &matrix_1, const Matrix3f &matrix_2);

            /// \brief Subtract 2 matrices
            ///
            /// This subtracts the matrix as follows: result = matrix1 - matrix2
            ///
            /// \param matrix_1 = First Matrix to subtract
            /// \param matrix_2 = Second Matrix to subtract
            /// \return The matrix
            static Matrix3f subtract(const Matrix3f &matrix_1, const Matrix3f &matrix_2);

            /// \brief Calculate the adjoint (or known as Adjugate or Conjugate Transpose) of a matrix
            ///
            /// \param matrix = The matrix to use
            /// \return The adjoint matrix
            static Matrix3f adjoint(const Matrix3f &matrix);

            /// \brief Calculate the matrix inverse of a matrix
            ///
            /// Creates a zero matrix if the determinent == 0
            /// \param matrix = The matrix to use
            /// \return The inversed matrix
            static Matrix3f inverse(const Matrix3f &matrix);

            /// \brief Calculate the transpose of a matrix
            ///
            /// \param matrix = The matrix to use
            /// \return The transposed matrix
            static Matrix3f transpose(const Matrix3f &matrix);

  
            float matrix[9];

  
            /// \brief Multiply this matrix and a specified matrix.
            ///
            /// This multiplies the matrix as follows: this = mult * this
            ///
            /// \param mult = Matrix to multiply
            ///
            /// \return reference to this object
            Matrix3f &multiply(const Matrix3f &mult);

            /// \brief Add this matrix and a specified matrix.
            ///
            /// This adds the matrix as follows: this = add_matrix + this
            ///
            /// \param add_matrix = Matrix to add
            ///
            /// \return reference to this object
            Matrix3f &add(const Matrix3f &add_matrix);

            /// \brief Subtract this matrix and a specified matrix.
            ///
            /// This subtracts the matrix as follows: this = sub_matrix - this
            ///
            /// \param sub_matrix = Matrix to subtract
            ///
            /// \return reference to this object
            Matrix3f &subtract(const Matrix3f &sub_matrix);

            /// \brief Calculate the matrix determinant
            double det() const;

            /// \brief Creates the adjoint (or known as adjugate) of the matrix
            ///
            /// \return reference to this object
            Matrix3f &adjoint();

            /// \brief Create the matrix inverse. (Returns a zero matrix if the determinent = 0)
            ///
            /// \return reference to this object
            Matrix3f &inverse();

            /// \brief Calculate the transpose of this matrix
            ///
            /// \return reference to this object
            Matrix3f &transpose();

   
    
            /// \brief Copy assignment operator.
			Matrix3f &operator =(const Matrix3f &copy) {for (size_t i=0;i<9;i++) matrix[i]=copy.matrix[i]; return *this; }

            /// \brief Copy assignment operator.
            Matrix3f &operator =(const Matrix2f &copy);

            /// \brief Copy assignment operator.
            Matrix3f &operator =(const Matrix4f &copy);

            /// \brief Multiplication operator.
            Matrix3f operator *(const Matrix3f &mult) const { Matrix3f result = *this; result.multiply(mult); return result; }

            /// \brief Addition operator.
            Matrix3f operator +(const Matrix3f &add_matrix) const { Matrix3f result = *this; result.add(add_matrix); return result; }

            /// \brief Subtraction operator.
            Matrix3f operator -(const Matrix3f &sub_matrix) const { Matrix3f result = *this; result.subtract(sub_matrix); return result; }

            /// \brief Equality operator.
            bool operator==(const Matrix3f &other)
            {
                    for (int i=0; i<9; i++)
                            if (matrix[i] != other.matrix[i]) return false;
                    return true;
            }

            /// \brief Not-equal operator.
            bool operator!=(const Matrix3f &other) { return !((*this) == other); }

  
			
    private:
    };


  



    /////////////////////////////////////////////////////////////////////////////
    // Matrix3f construction:


    inline Matrix3f::Matrix3f(const Matrix2f &copy)
    {
            matrix[0*3 + 0] = copy.matrix[0*2 + 0];
            matrix[0*3 + 1] = copy.matrix[0*2 + 1];
            matrix[0*3 + 2] = 0;
            matrix[1*3 + 0] = copy.matrix[1*2 + 0];
            matrix[1*3 + 1] = copy.matrix[1*2 + 1];
            matrix[1*3 + 2] = 0;
            matrix[2*3 + 0] = 0;
            matrix[2*3 + 1] = 0;
            matrix[2*3 + 2] = 1;
    }

    inline Matrix3f Matrix3f::null()
    {
		Matrix3f m;
		for (size_t i=0;i<3*3;i++) {
			m.matrix[i]=0.0f;
		}
		return m;
    }

    inline Matrix3f Matrix3f::identity()
    {
            Matrix3f m = null();
            m.matrix[0] = 1;
            m.matrix[4] = 1;
            m.matrix[8] = 1;
            return m;
    }

    inline Matrix3f Matrix3f::rotate( float angle, float x, float y, float z, bool normalize)
    {
            if (normalize)
            {
                    float len2 = x*x+y*y+z*z;
                    if (len2 != (float)1)
                    {
						float length = sqrt(len2);
                            if (length > (float) 0)
                            {
                                    x /= length;
                                    y /= length;
                                    z /= length;
                            }
                            else
                            {
                                    x = (float) 0;
                                    y = (float) 0;
                                    z = (float) 0;
                            }
                    }
            }

            Matrix3f rotate_matrix;
		float c = cos(angle);
		float s = sin(angle);
            rotate_matrix.matrix[0+0*3] = (float) (x*x*(1.0f - c) + c);
            rotate_matrix.matrix[0+1*3] = (float) (x*y*(1.0f - c) - z*s);
            rotate_matrix.matrix[0+2*3] = (float) (x*z*(1.0f - c) + y*s);
            rotate_matrix.matrix[1+0*3] = (float) (y*x*(1.0f - c) + z*s);
            rotate_matrix.matrix[1+1*3] = (float) (y*y*(1.0f - c) + c);
            rotate_matrix.matrix[1+2*3] = (float) (y*z*(1.0f - c) - x*s);
            rotate_matrix.matrix[2+0*3] = (float) (x*z*(1.0f - c) - y*s);
            rotate_matrix.matrix[2+1*3] = (float) (y*z*(1.0f - c) + x*s);
            rotate_matrix.matrix[2+2*3] = (float) (z*z*(1.0f - c) + c);
            return rotate_matrix;
    }


    inline Matrix3f Matrix3f::multiply(const Matrix3f &matrix_1, const Matrix3f &matrix_2)
    {
            Matrix3f dest(matrix_2);
            dest.multiply(matrix_1);
            return dest;
    }

    inline Matrix3f Matrix3f::add(const Matrix3f &matrix_1, const Matrix3f &matrix_2)
    {
            Matrix3f dest(matrix_2);
            dest.add(matrix_1);
            return dest;
    }

    inline Matrix3f Matrix3f::subtract(const Matrix3f &matrix_1, const Matrix3f &matrix_2)
    {
            Matrix3f dest(matrix_2);
            dest.subtract(matrix_1);
            return dest;
    }

    inline Matrix3f Matrix3f::adjoint(const Matrix3f &matrix)
    {
            Matrix3f dest(matrix);
            dest.adjoint();
            return dest;
    }

    inline Matrix3f Matrix3f::inverse(const Matrix3f &matrix)
    {
            Matrix3f dest(matrix);
            dest.inverse();
            return dest;
    }

    inline Matrix3f Matrix3f::transpose(const Matrix3f &matrix)
    {
            Matrix3f dest(matrix);
            dest.transpose();
            return dest;
    }

    /////////////////////////////////////////////////////////////////////////////
    // Matrix3f attributes:

    /////////////////////////////////////////////////////////////////////////////
    // Matrix3f operators:


    inline Matrix3f &Matrix3f::operator =(const Matrix2f &copy)
    {
            matrix[0*3 + 0] = copy.matrix[0*2 + 0];
            matrix[0*3 + 1] = copy.matrix[0*2 + 1];
            matrix[0*3 + 2] = 0;
            matrix[1*3 + 0] = copy.matrix[1*2 + 0];
            matrix[1*3 + 1] = copy.matrix[1*2 + 1];
            matrix[1*3 + 2] = 0;
            matrix[2*3 + 0] = 0;
            matrix[2*3 + 1] = 0;
            matrix[2*3 + 2] = 1;
            return *this;
    }


    /////////////////////////////////////////////////////////////////////////////
    // Matrix3f operations:

    inline Matrix3f &Matrix3f::multiply(const Matrix3f &mult)
    {
            Matrix3f result;
            for (int x=0; x<3; x++)
            {
                    for (int y=0; y<3; y++)
                    {
                            result.matrix[x+y*3] =
                                    matrix[0*3 + x]*mult.matrix[y*3 + 0] +
                                    matrix[1*3 + x]*mult.matrix[y*3 + 1] +
                                    matrix[2*3 + x]*mult.matrix[y*3 + 2];

                    }
            }
            *this = result;
            return *this;
    }

    inline Matrix3f &Matrix3f::add(const Matrix3f &add_matrix)
    {
            matrix[(0*3) + 0] += add_matrix.matrix[(0*3) + 0];
            matrix[(0*3) + 1] += add_matrix.matrix[(0*3) + 1];
            matrix[(0*3) + 2] += add_matrix.matrix[(0*3) + 2];
            matrix[(1*3) + 0] += add_matrix.matrix[(1*3) + 0];
            matrix[(1*3) + 1] += add_matrix.matrix[(1*3) + 1];
            matrix[(1*3) + 2] += add_matrix.matrix[(1*3) + 2];
            matrix[(2*3) + 0] += add_matrix.matrix[(2*3) + 0];
            matrix[(2*3) + 1] += add_matrix.matrix[(2*3) + 1];
            matrix[(2*3) + 2] += add_matrix.matrix[(2*3) + 2];
            return *this;
    }

    inline Matrix3f &Matrix3f::subtract(const Matrix3f &sub_matrix)
    {
            matrix[(0*3) + 0] = sub_matrix.matrix[(0*3) + 0] - matrix[(0*3) + 0];
            matrix[(0*3) + 1] = sub_matrix.matrix[(0*3) + 1] - matrix[(0*3) + 1];
            matrix[(0*3) + 2] = sub_matrix.matrix[(0*3) + 2] - matrix[(0*3) + 2];
            matrix[(1*3) + 0] = sub_matrix.matrix[(1*3) + 0] - matrix[(1*3) + 0];
            matrix[(1*3) + 1] = sub_matrix.matrix[(1*3) + 1] - matrix[(1*3) + 1];
            matrix[(1*3) + 2] = sub_matrix.matrix[(1*3) + 2] - matrix[(1*3) + 2];
            matrix[(2*3) + 0] = sub_matrix.matrix[(2*3) + 0] - matrix[(2*3) + 0];
            matrix[(2*3) + 1] = sub_matrix.matrix[(2*3) + 1] - matrix[(2*3) + 1];
            matrix[(2*3) + 2] = sub_matrix.matrix[(2*3) + 2] - matrix[(2*3) + 2];
            return *this;
    }

    inline double Matrix3f::det() const
    {
            double value;

            value  = matrix[0*3 + 0] * ( (matrix[1*3 + 1] * matrix[2*3 + 2]) - (matrix[2*3 + 1] * matrix[1*3 + 2]) );
            value -= matrix[0*3 + 1] * ( (matrix[1*3 + 0] * matrix[2*3 + 2]) - (matrix[2*3 + 0] * matrix[1*3 + 2]) );
            value += matrix[0*3 + 2] * ( (matrix[1*3 + 0] * matrix[2*3 + 1]) - (matrix[2*3 + 0] * matrix[1*3 + 1]) );

            return value;
    }

    inline Matrix3f &Matrix3f::adjoint()
    {
            Matrix3f result;
            result.matrix[ 0*3 + 0 ] =  ( ( matrix[1*3 + 1] * matrix[2*3 + 2] ) - ( matrix[1*3 + 2] * matrix[2*3 + 1] ) );
            result.matrix[ 1*3 + 0 ] = -( ( matrix[1*3 + 0] * matrix[2*3 + 2] ) - ( matrix[1*3 + 2] * matrix[2*3 + 0] ) );
            result.matrix[ 2*3 + 0 ] =  ( ( matrix[1*3 + 0] * matrix[2*3 + 1] ) - ( matrix[1*3 + 1] * matrix[2*3 + 0] ) );
            result.matrix[ 0*3 + 1 ] = -( ( matrix[0*3 + 1] * matrix[2*3 + 2] ) - ( matrix[0*3 + 2] * matrix[2*3 + 1] ) );
            result.matrix[ 1*3 + 1 ] =  ( ( matrix[0*3 + 0] * matrix[2*3 + 2] ) - ( matrix[0*3 + 2] * matrix[2*3 + 0] ) );
            result.matrix[ 2*3 + 1 ] = -( ( matrix[0*3 + 0] * matrix[2*3 + 1] ) - ( matrix[0*3 + 1] * matrix[2*3 + 0] ) );
            result.matrix[ 0*3 + 2 ] =  ( ( matrix[0*3 + 1] * matrix[1*3 + 2] ) - ( matrix[0*3 + 2] * matrix[1*3 + 1] ) );
            result.matrix[ 1*3 + 2 ] = -( ( matrix[0*3 + 0] * matrix[1*3 + 2] ) - ( matrix[0*3 + 2] * matrix[1*3 + 0] ) );
            result.matrix[ 2*3 + 2 ] =  ( ( matrix[0*3 + 0] * matrix[1*3 + 1] ) - ( matrix[0*3 + 1] * matrix[1*3 + 0] ) );
            *this = result;
            return *this;
    }

    inline Matrix3f &Matrix3f::inverse()
    {
            double d;

            d = det();

            // Inverse unknown when determinant is close to zero
            if (fabs(d) < 1e-15)
            {
                    *this = null();
            }
            else
            {
                    Matrix3f result = *this;
                    result.adjoint();

                    d=1.0/d;	// Inverse the determinant
                    for (int i=0; i<9; i++)
                    {
                            result.matrix[i] = (float) (result.matrix[i] * d);
                    }

                    *this = result;
            }
            return *this;
    }

    inline Matrix3f &Matrix3f::transpose()
    {
            float original[9];
            for (int cnt=0; cnt<9; cnt++)
                    original[cnt] = matrix[cnt];

            matrix[0] = original[0];
            matrix[1] = original[3];
            matrix[2] = original[6];
            matrix[3] = original[1];
            matrix[4] = original[4];
            matrix[5] = original[7];
            matrix[6] = original[2];
            matrix[7] = original[5];
            matrix[8] = original[8];

            return *this;
    }

	
	/// Matrix is assumed to be in the Column-Major matrix format (opengl native)\n
    /// Note: "vec = vector * matrix"  is different to "vec = matrix * vector"
    inline Vector3f operator * (const Vector3f& v, const Matrix3f& m)
    {
		return Vector3f(
						m.matrix[0*3+0]*v.x + m.matrix[0*3+1]*v.y + m.matrix[0*3+2]*v.z,
						m.matrix[1*3+0]*v.x + m.matrix[1*3+1]*v.y + m.matrix[1*3+2]*v.z,
						m.matrix[2*3+0]*v.x + m.matrix[2*3+1]*v.y + m.matrix[2*3+2]*v.z);
    }
	
    /// Matrix is assumed to be in the Column-Major matrix format (opengl native)\n
    /// Note: "vec = vector * matrix"  is different to "vec = matrix * vector"
    inline Vector3f operator * (const Matrix3f& m, const Vector3f& v)
    {
		return Vector3f(
						m.matrix[0*3+0]*v.x + m.matrix[1*3+0]*v.y + m.matrix[2*3+0]*v.z,
						m.matrix[0*3+1]*v.x + m.matrix[1*3+1]*v.y + m.matrix[2*3+1]*v.z,
						m.matrix[0*3+2]*v.x + m.matrix[1*3+2]*v.y + m.matrix[2*3+2]*v.z);
    }
	
    /////////////////////////////////////////////////////////////////////////////
    // Matrix3f implementation:

  

}

#endif /*SB_MATRIX3_H_INCLUDED*/
