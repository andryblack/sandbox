#ifndef SB_MATRIX4_H_INCLUDED
#define SB_MATRIX4_H_INCLUDED



#include "sb_matrix2.h"
#include "sb_matrix3.h"
#include "sb_vector3.h"
#include "sb_vector4.h"
#include <sbstd/sb_assert.h>

#include <float.h>

namespace Sandbox {

   
    struct Matrix4f;

	enum EulerOrder {
		EULER_ORDER_XYZ,
		EULER_ORDER_XZY,
		EULER_ORDER_YZX,
		EULER_ORDER_YXZ,
		EULER_ORDER_ZXY,
		EULER_ORDER_ZYX,
	};

    
    /// \brief 4D matrix
    ///
    struct Matrix4f
    {
  
            /// \brief Constructs a 4x4 matrix (null)
            Matrix4f()
            {
                    for (int i=0; i<16; i++)
                            matrix[i] = 0;
            }

            /// \brief Constructs a 4x4 matrix (copied)
            Matrix4f(const Matrix4f &copy)
            {
                    for (int i=0; i<16; i++)
                            matrix[i] = copy.matrix[i];
            }

            /// \brief Constructs a 4x4 matrix (copied from a 2d matrix)
            explicit Matrix4f(const Matrix2f &copy);

            /// \brief Constructs a 4x4 matrix (copied from a 3d matrix)
            explicit Matrix4f(const Matrix3f &copy);

            /// \brief Constructs a 4x4 matrix (copied from a array of floats)
            explicit Matrix4f(const float *init_matrix)
            {
                    for (int i=0; i<16; i++)
                            matrix[i] = (float) init_matrix[i];
            }



            /// \brief Create a zero matrix
            ///
            /// \return The matrix
            static Matrix4f null();

            /// \brief Create the identity matrix
            /// \return The matrix
            static Matrix4f identity();

            /// \brief Create a frustum matrix
            ///
            /// Matrix is created in the Column-Major matrix format (opengl native)
            /// \return The matrix (in column-major format)
            static Matrix4f frustum(float left, float right, float bottom, float top, float z_near, float z_far);

            /// \brief Create a perspective matrix
            ///
            /// Matrix is created in the Column-Major matrix format (opengl native)
            /// \return The matrix (in column-major format)
            static Matrix4f perspective(
                    float field_of_view_y_degrees,
                    float aspect,
                    float z_near,
                    float z_far);

            /// \brief Create a ortho matrix
            ///
            /// Matrix is created in the Column-Major matrix format (opengl native)
            /// \return The matrix (in column-major format)
            static Matrix4f ortho(float left, float right, float bottom, float top, float z_near, float z_far);

            /// \brief Create a ortho_2d matrix
            ///
            /// Matrix is created in the Column-Major matrix format (opengl native)
            /// \return The matrix (in column-major format)
            static Matrix4f ortho_2d(float left, float right, float bottom, float top);

            /// \brief Create a rotation matrix
            ///
            /// Matrix is created in the Column-Major matrix format (opengl native)
            /// \param angle = Angle to rotate by
            /// \param x = Amount to rotate in the X axis
            /// \param y = Amount to rotate in the Y axis
            /// \param z = Amount to rotate in the Z axis
            /// \param normalize = true = Normalize x,y,z before creating rotation matrix
            /// \return The matrix (in column-major format)
            static Matrix4f rotate(float angle, float x, float y, float z, bool normalize = true);

            /// \brief Create a rotation matrix using XYZ euler angles
            ///
            /// Matrix is created in the Column-Major matrix format (opengl native)
            ///
            /// \return The matrix (in column-major format)
            static Matrix4f rotate(float angle_x, float angle_y, float angle_z, EulerOrder euler);

            static inline Matrix4f rotate(const Sandbox::Vector3f& v, EulerOrder euler) {
                return rotate(v.x, v.y, v.z, euler);
            }
            /// \brief Create a scale matrix
            ///
            /// \param x = Scale X
            /// \param y = Scale Y
            /// \param z = Scale Z
            /// \return The matrix
            static Matrix4f scale(float x, float y, float z);

            /// \brief Create a translation matrix
            ///
            /// Matrix is created in the Column-Major matrix format (opengl native)
            /// \param x = Translate X
            /// \param y = Translate Y
            /// \param z = Translate Z
            /// \return The matrix (in column-major format)
            static Matrix4f translate(float x, float y, float z);
        
            static inline Matrix4f translate(const Vector3f& v) {
                return translate(v.x, v.y, v.z);
            }

            /// \brief Create the "look at" matrix
            ///
            /// Matrix is created in the Column-Major matrix format (opengl native)
            /// \param eye_x = Eye position X
            /// \param eye_y = Eye position Y
            /// \param eye_z = Eye position Z
            /// \param center_x = Center X
            /// \param center_y = Center Y
            /// \param center_z = Center Z
            /// \param up_x = Translation X
            /// \param up_y = Translation Y
            /// \param up_z = Translation Z
            /// \return The matrix (in column-major format)
            static Matrix4f look_at(
                    float eye_x, float eye_y, float eye_z,
                    float center_x, float center_y, float center_z,
                    float up_x, float up_y, float up_z);

            /// \brief Multiply 2 matrices
            ///
            /// This multiplies the matrix as follows: result = matrix1 * matrix2
            ///
            /// \param matrix_1 = First Matrix to multiply
            /// \param matrix_2 = Second Matrix to multiply
            /// \return The matrix
            static Matrix4f multiply(const Matrix4f &matrix_1, const Matrix4f &matrix_2);

            /// \brief Add 2 matrices
            ///
            /// This adds the matrix as follows: result = matrix1 + matrix2
            ///
            /// \param matrix_1 = First Matrix to add
            /// \param matrix_2 = Second Matrix to add
            /// \return The matrix
            static Matrix4f add(const Matrix4f &matrix_1, const Matrix4f &matrix_2);

            /// \brief Subtract 2 matrices
            ///
            /// This subtracts the matrix as follows: result = matrix1 - matrix2
            ///
            /// \param matrix_1 = First Matrix to subtract
            /// \param matrix_2 = Second Matrix to subtract
            /// \return The matrix
            static Matrix4f subtract(const Matrix4f &matrix_1, const Matrix4f &matrix_2);

            /// \brief Calculate the adjoint (or known as Adjugate or Conjugate Transpose) of a matrix
            ///
            /// \param matrix = The matrix to use
            /// \return The adjoint matrix
            static Matrix4f adjoint(const Matrix4f &matrix);

            /// \brief Calculate the matrix inverse of a matrix
            ///
            /// Creates a zero matrix if the determinent == 0
            /// \param matrix = The matrix to use
            /// \return The inversed matrix
            static Matrix4f inverse(const Matrix4f &matrix);

            /// \brief Calculate the transpose of a matrix
            ///
            /// \param matrix = The matrix to use
            /// \return The transposed matrix
            static Matrix4f transpose(const Matrix4f &matrix);

   
            /// The matrix (in column-major format)
            float matrix[16];

            /// \brief Returns the x coordinate for the point (0,0,0) multiplied with this matrix.
            float get_origin_x() const { return matrix[12]; }

            /// \brief Returns the y coordinate for the point (0,0,0) multiplied with this matrix.
            float get_origin_y() const { return matrix[13]; }

            /// \brief Returns the z coordinate for the point (0,0,0) multiplied with this matrix.
            float get_origin_z() const { return matrix[14]; }

            /// \brief Extract the euler angles (in radians) from a matrix (in column-major format)
            ///
            /// \return The x,y,z angles (in radians)
            Vector3f get_euler(EulerOrder euler) const;

            /// \brief Get a transformed point from the matrix (in column-major format)
            ///
            /// \return The transformed point
            Vector3f transform_point(const Vector3f &vector) const;

  
            /// \brief Multiply this matrix and a specified matrix.
            ///
            /// This multiplies the matrix as follows: this = mult * this
            ///
            /// \param mult = Matrix to multiply
            ///
            /// \return reference to this object
            Matrix4f &multiply(const Matrix4f &mult);

            /// \brief Add this matrix and a specified matrix.
            ///
            /// This adds the matrix as follows: this = mult + this
            ///
            /// \param add_matrix = Matrix to add
            ///
            /// \return reference to this object
            Matrix4f &add(const Matrix4f &add_matrix);

            /// \brief Subtract this matrix and a specified matrix.
            ///
            /// This subtracts the matrix as follows: this = mult - this
            ///
            /// \param sub_matrix = Matrix to subtract
            ///
            /// \return reference to this object
            Matrix4f &subtract(const Matrix4f &sub_matrix);

            /// \brief Scale this matrix
            ///
            /// This is faster than using: multiply(Matrix4f::scale(x,y,z) )
            ///
            /// \param x = Scale X
            /// \param y = Scale Y
            /// \param z = Scale Z
            ///
            /// \return reference to this object
            Matrix4f &scale_self(float x, float y, float z);
            Matrix4f &scale_self(const Vector3f& v) {
                return scale_self(v.x,v.y,v.z);
            }

            /// \brief Translate this matrix
            ///
            /// Matrix is assumed to be in the Column-Major matrix format (opengl native)\n
            /// This is faster than using: multiply(Matrix4f::translate(x,y,z) )
            ///
            /// \param x = Translate X
            /// \param y = Translate Y
            /// \param z = Translate Z
            ///
            /// \return reference to this object
            Matrix4f &translate_self(float x, float y, float z);

            Matrix4f &translate_self(const Vector3f& v ) {
                return translate_self(v.x,v.y,v.z);
            }

            /// \brief Calculate the matrix determinant of this matrix
            ///
            /// \return The determinant
            double det() const;

            /// \brief Calculate the adjoint (or known as adjugate) of this matrix
            ///
            /// \return reference to this object
            Matrix4f &adjoint();

            /// \brief Calculate the matrix inverse of this matrix
            ///
            /// Creates a zero matrix if the determinent == 0
            /// \return reference to this object
            Matrix4f &inverse();

            /// \brief Calculate the transpose of this matrix
            ///
            /// \return reference to this object
            Matrix4f &transpose();

   
            /// \brief Copy assignment operator.
            Matrix4f &operator =(const Matrix4f &copy) {
				for (size_t i=0;i<4*4;i++)
					matrix[i]=copy.matrix[i];
				return *this; 
			}

            /// \brief Multiplication operator.
            Matrix4f operator *(const Matrix4f &mult) const { Matrix4f result = *this; result.multiply(mult); return result; }

            Matrix4f operator *=(const Matrix4f &mult) { multiply(mult); return (*this); }

        
            /// \brief Addition operator.
            Matrix4f operator +(const Matrix4f &add_matrix) const { Matrix4f result = *this; result.add(add_matrix); return result; }

            /// \brief Subtraction operator.
            Matrix4f operator -(const Matrix4f &sub_matrix) const { Matrix4f result = *this; result.subtract(sub_matrix); return result; }

			float get(size_t x,size_t y) const { return matrix[x+y*4];}
			void set(size_t x,size_t y,float val) { matrix[x+y*4]=val;} 
		
		Matrix4f inverted() const {
			Matrix4f res = *this;
			res.inverse();
			return res;
		}
		Matrix4f mul(const Matrix4f& m) const {
			Matrix4f res = *this;
			res.multiply(m);
			return res;
		}
   private:
    };
































    inline Matrix4f &Matrix4f::multiply(const Matrix4f &mult)
    {
			Matrix4f result;
            for (int x=0; x<4; x++)
            {
                    for (int y=0; y<4; y++)
                    {
                            result.matrix[x+y*4] =
                                    matrix[0*4 + x]*mult.matrix[y*4 + 0] +
                                    matrix[1*4 + x]*mult.matrix[y*4 + 1] +
                                    matrix[2*4 + x]*mult.matrix[y*4 + 2] +
                                    matrix[3*4 + x]*mult.matrix[y*4 + 3];
                    }
            }
            *this = result;
            return *this;
    }


    /////////////////////////////////////////////////////////////////////////////
    // Matrix4f construction:

    inline Matrix4f::Matrix4f(const Matrix3f &copy)
    {
            matrix[0*4 + 0] = copy.matrix[0*3 + 0];
            matrix[0*4 + 1] = copy.matrix[0*3 + 1];
            matrix[0*4 + 2] = copy.matrix[0*3 + 2];
            matrix[0*4 + 3] = 0;
            matrix[1*4 + 0] = copy.matrix[1*3 + 0];
            matrix[1*4 + 1] = copy.matrix[1*3 + 1];
            matrix[1*4 + 2] = copy.matrix[1*3 + 2];
            matrix[1*4 + 3] = 0;
            matrix[2*4 + 0] = copy.matrix[2*3 + 0];
            matrix[2*4 + 1] = copy.matrix[2*3 + 1];
            matrix[2*4 + 2] = copy.matrix[2*3 + 2];
            matrix[2*4 + 3] = 0;
            matrix[3*4 + 0] = 0;
            matrix[3*4 + 1] = 0;
            matrix[3*4 + 2] = 0;
            matrix[3*4 + 3] = 1;
    }

    inline Matrix4f::Matrix4f(const Matrix2f &copy)
    {
            matrix[0*4 + 0] = copy.matrix[0*2 + 0];
            matrix[0*4 + 1] = copy.matrix[0*2 + 1];
            matrix[0*4 + 2] = 0;
            matrix[0*4 + 3] = 0;
            matrix[1*4 + 0] = copy.matrix[1*2 + 0];
            matrix[1*4 + 1] = copy.matrix[1*2 + 1];
            matrix[1*4 + 2] = 0;
            matrix[1*4 + 3] = 0;
            matrix[2*4 + 0] = 0;
            matrix[2*4 + 1] = 0;
            matrix[2*4 + 2] = 1;
            matrix[2*4 + 3] = 0;
            matrix[3*4 + 0] = 0;
            matrix[3*4 + 1] = 0;
            matrix[3*4 + 2] = 0;
            matrix[3*4 + 3] = 1;
    }

    inline Matrix4f Matrix4f::null()
    {
		Matrix4f m;
		for (size_t i=0;i<4*4;i++) m.matrix[i]=0.0f;
		return m;
    }

    inline Matrix4f Matrix4f::identity()
    {
            Matrix4f m = null();
            m.matrix[0] = 1;
            m.matrix[5] = 1;
            m.matrix[10] = 1;
            m.matrix[15] = 1;
            return m;
    }

    inline Matrix4f Matrix4f::frustum(float  left, float  right, float  bottom, float  top, float  z_near, float  z_far)
    {
            Matrix4f frustum_matrix = null();
            float  a = (right + left) / (right - left);
            float  b = (top + bottom) / (top - bottom);
            float  c = -(z_far + z_near) / (z_far - z_near);
            float  d = -(2 * z_far * z_near) / (z_far - z_near);
            frustum_matrix.matrix[0+0*4] = (float) (2*z_near / (right - left));
            frustum_matrix.matrix[1+1*4] = (float) (2*z_near / (top - bottom));
            frustum_matrix.matrix[0+2*4] = (float) a;
            frustum_matrix.matrix[1+2*4] = (float) b;
            frustum_matrix.matrix[2+2*4] = (float) c;
            frustum_matrix.matrix[2+3*4] = (float) d;
            frustum_matrix.matrix[3+2*4] = (float) -1;
            return frustum_matrix;
    }


    // For floats
    inline Matrix4f Matrix4f::perspective(
            float field_of_view_y_degrees,
            float aspect,
            float z_near,
            float z_far)
    {
            Matrix4f projection_matrix = null();
            float field_of_view_y_rad = field_of_view_y_degrees * float(M_PI / 180.0f);
		float f = 1.0f / std::tan(field_of_view_y_rad / 2.0f);
            projection_matrix.matrix[0+0*4] = (f/aspect);
            projection_matrix.matrix[1+1*4] = f;
            projection_matrix.matrix[2+2*4] = ((z_far + z_near) / (z_near - z_far));
            projection_matrix.matrix[2+3*4] = ((2.0f * z_far * z_near) / (z_near - z_far));
            projection_matrix.matrix[3+2*4] = -1.0f;
            return projection_matrix;
    }

    inline Matrix4f Matrix4f::ortho(float left, float right, float bottom, float top, float z_near, float z_far)
    {
            Matrix4f ortho_matrix = null();
            float tx = -(right + left) / (right - left);
            float ty = -(top + bottom) / (top - bottom);
            float tz = -(z_far + z_near) / (z_far - z_near);
            ortho_matrix.matrix[0+0*4] = (float) (2 / (right - left));
            ortho_matrix.matrix[1+1*4] = (float) (2 / (top - bottom));
            ortho_matrix.matrix[2+2*4] = (float) (-2 / (z_far - z_near));
            ortho_matrix.matrix[0+3*4] = (float) tx;
            ortho_matrix.matrix[1+3*4] = (float) ty;
            ortho_matrix.matrix[2+3*4] = (float) tz;
            ortho_matrix.matrix[3+3*4] = (float) 1;
            return ortho_matrix;
    }

    inline Matrix4f Matrix4f::ortho_2d(float left, float right, float bottom, float top)
    {
            return ortho(left, right, bottom, top, (float)-1.0, (float)1.0);
    }

    inline Matrix4f Matrix4f::rotate(float angle, float x, float y, float z, bool normalize)
    {
        Matrix4f rotate_matrix = identity();
        float c = cos(angle);
        float s = sin(angle);

        if (x == 0.0f) {
              if (y == 0.0f) {
                 if (z != 0.0f) {
                    /* rotate only around z-axis */
                    rotate_matrix.matrix[0+0*4] = c;
                    rotate_matrix.matrix[1+1*4] = c;
                    if (z < 0.0F) {
                       rotate_matrix.matrix[0+1*4] = s;
                       rotate_matrix.matrix[1+0*4] = -s;
                    }
                    else {
                       rotate_matrix.matrix[0+1*4] = -s;
                       rotate_matrix.matrix[1+0*4] = s;
                    }
                    return rotate_matrix;
                 }
              }
              else if (z == 0.0F) {
                 /* rotate only around y-axis */
                 rotate_matrix.matrix[0+0*4] = c;
                 rotate_matrix.matrix[2+2*4] = c;
                 if (y < 0.0F) {
                    rotate_matrix.matrix[0+2*4] = -s;
                    rotate_matrix.matrix[2+0*4] = s;
                 }
                 else {
                    rotate_matrix.matrix[0+2*4] = s;
                    rotate_matrix.matrix[2+0*4] = -s;
                 }
                 return rotate_matrix;
              }
           }
           else if (y == 0.0F) {
              if (z == 0.0F) {
                 /* rotate only around x-axis */
                 rotate_matrix.matrix[1+1*4] = c;
                 rotate_matrix.matrix[2+2*4] = c;
                 if (x < 0.0F) {
                    rotate_matrix.matrix[1+2*4] = s;
                    rotate_matrix.matrix[2+1*4] = -s;
                 }
                 else {
                    rotate_matrix.matrix[1+2*4] = -s;
                    rotate_matrix.matrix[2+1*4] = s;
                 }
                 return rotate_matrix;
              }
           }

            if (normalize)
            {
                    float len2 = x*x+y*y+z*z;
                    if (len2 != (float)1)
                    {
						float length = std::sqrt(len2);
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



            float c_one = 1.0f - c;
            float xx = x * x;
            float xy = x * y;
            float xz = x * z;
            float yy = y * y;
            float yz = y * z;
            float zz = z * z;

            float xs = x * s;
            float ys = y * s;
            float zs = z * s;

            rotate_matrix.matrix[0+0*4] = c_one*xx + c;
            rotate_matrix.matrix[0+1*4] = c_one*xy - zs;
            rotate_matrix.matrix[0+2*4] = c_one*xz + ys;

            rotate_matrix.matrix[1+0*4] = c_one*xy + zs;
            rotate_matrix.matrix[1+1*4] = c_one*yy + c;
            rotate_matrix.matrix[1+2*4] = c_one*yz - xs;

            rotate_matrix.matrix[2+0*4] = c_one*xz - ys;
            rotate_matrix.matrix[2+1*4] = c_one*yz + xs;
            rotate_matrix.matrix[2+2*4] = c_one*zz + c;

            rotate_matrix.matrix[3+3*4] = 1;
            return rotate_matrix;
    }

    inline Matrix4f Matrix4f::rotate(float angle_x, float angle_y, float angle_z, EulerOrder order)
    {
            Matrix4f rotation_matrix_x = Matrix4f::rotate(angle_x, 1.0f, 0.0f, 0.0f, false);
            Matrix4f rotation_matrix_y = Matrix4f::rotate(angle_y, 0.0f, 1.0f, 0.0f, false);
            Matrix4f rotation_matrix_z = Matrix4f::rotate(angle_z, 0.0f, 0.0f, 1.0f, false);

            switch (order)
            {
                    case EULER_ORDER_XYZ:
                            return rotation_matrix_z * rotation_matrix_y * rotation_matrix_x;
                    case EULER_ORDER_XZY:
                            return rotation_matrix_y * rotation_matrix_z * rotation_matrix_x;
                    case EULER_ORDER_YZX:
                            return rotation_matrix_x * rotation_matrix_z * rotation_matrix_y;
                    case EULER_ORDER_YXZ:
                            return rotation_matrix_z * rotation_matrix_x * rotation_matrix_y;
                    case EULER_ORDER_ZXY:
                            return rotation_matrix_y * rotation_matrix_x * rotation_matrix_z;
                    case EULER_ORDER_ZYX:
                            return rotation_matrix_x * rotation_matrix_y * rotation_matrix_z;
                    default:
                            sb_assert( false && "Unknown euler order");
            }
            return Matrix4f();

    }

    inline Vector3f Matrix4f::get_euler(EulerOrder order) const
    {
            Vector3f angles;

            int pos_i;
            int pos_j;
            int pos_k;

            // Obtain the correct xyz rotation position from the original rotation order
            switch (order)
            {
                    case EULER_ORDER_XYZ:
                            pos_i = 0; pos_j = 1; pos_k = 2; break;
                    case EULER_ORDER_XZY:
                            pos_i = 0; pos_j = 2; pos_k = 1; break;
                    case EULER_ORDER_YZX:
                            pos_i = 2; pos_j = 0; pos_k = 1; break;
                    case EULER_ORDER_YXZ:
                            pos_i = 1; pos_j = 0; pos_k = 2; break;
                    case EULER_ORDER_ZXY:
                            pos_i = 1; pos_j = 2; pos_k = 0; break;
                    case EULER_ORDER_ZYX:
                            pos_i = 2; pos_j = 1; pos_k = 0; break;
                    default:
                            sb_assert(false && "Unknown euler order");
            }

            float cy = float(sqrt(matrix[ (4*pos_i) + pos_i ]*matrix[ (4*pos_i) + pos_i ] + matrix[ (4*pos_j) + pos_i ]*matrix[ (4*pos_j) + pos_i ]));
            if (cy > (float) 16.0*FLT_EPSILON)
            {
				angles.x = std::atan2(matrix[ (4*pos_k) + pos_j ], matrix[ (4*pos_k) + pos_k ]);
                    angles.y = std::atan2(-matrix[ (4*pos_k) + pos_i ], cy);
                    angles.z = std::atan2(matrix[ (4*pos_j) + pos_i ], matrix[ (4*pos_i) + pos_i ]);
            }
            else
            {
                    angles.x = std::atan2(-matrix[ (4*pos_j) + pos_k ], matrix[ (4*pos_j) + pos_j ]);
                    angles.y = std::atan2(-matrix[ (4*pos_k) + pos_i ], cy);
                    angles.z = 0;
            }

            // Swap the xyz value to the specified euler angle
            switch (order)
            {
                    case EULER_ORDER_XYZ:
                            break;
                    case EULER_ORDER_XZY:
                            angles = Vector3f(angles.x, angles.z, angles.y);
                            break;
                    case EULER_ORDER_YZX:
                            angles = Vector3f(angles.y, angles.z, angles.x);
                            break;
                    case EULER_ORDER_YXZ:
                            angles = Vector3f(angles.y, angles.x, angles.z);
                            break;
                    case EULER_ORDER_ZXY:
                            angles = Vector3f(angles.z, angles.x, angles.y);
                            break;
                    case EULER_ORDER_ZYX:
                            angles = Vector3f(angles.z, angles.y, angles.x);
                            break;
            }

            return angles;
    }

    inline Vector3f Matrix4f::transform_point(const Vector3f &vector) const
    {
            Vector3f dest;

            dest.x = vector.x * matrix[0 + 0*4] +
                            vector.y * matrix[0 + 1*4] +
                            vector.z * matrix[0 + 2*4] +
                            matrix[0 + 3*4];

            dest.y = vector.x * matrix[1 + 0*4] +
                            vector.y * matrix[1 + 1*4] +
                            vector.z * matrix[1 + 2*4] +
                            matrix[1 + 3*4];

            dest.z = vector.x * matrix[2 + 0*4] +
                            vector.y * matrix[2 + 1*4] +
                            vector.z * matrix[2 + 2*4] +
                            matrix[2 + 3*4];

            float w = vector.x * matrix[3 + 0*4] +
                            vector.y * matrix[3 + 1*4] +
                            vector.z * matrix[3 + 2*4] +
                            matrix[3 + 3*4];

            if (w != (float) 0.0)
            {
                    dest.x /= w;
                    dest.y /= w;
                    dest.z /= w;
            }

            return dest;
    }

    inline Matrix4f Matrix4f::scale(float x, float y, float z)
    {
            Matrix4f scale_matrix = null();
            scale_matrix.matrix[0+0*4] = x;
            scale_matrix.matrix[1+1*4] = y;
            scale_matrix.matrix[2+2*4] = z;
            scale_matrix.matrix[3+3*4] = 1;
            return scale_matrix;
    }

    inline Matrix4f Matrix4f::translate(float x, float y, float z)
    {
            Matrix4f translate_matrix = identity();
            translate_matrix.matrix[0+3*4] = x;
            translate_matrix.matrix[1+3*4] = y;
            translate_matrix.matrix[2+3*4] = z;
            return translate_matrix;
    }



    // For floats
    inline Matrix4f Matrix4f::look_at(
            float eye_x, float eye_y, float eye_z,
            float center_x, float center_y, float center_z,
            float up_x, float up_y, float up_z)
    {
            float f[3] = { center_x - eye_x, center_y - eye_y, center_z - eye_z };
            float length_f = std::sqrt(f[0]*f[0] + f[1]*f[1] + f[2]*f[2]);
            f[0] /= length_f;
            f[1] /= length_f;
            f[2] /= length_f;

            float up[3] = { up_x, up_y, up_z };
            float length_up = std::sqrt(up[0]*up[0] + up[1]*up[1] + up[2]*up[2]);
            up[0] /= length_up;
            up[1] /= length_up;
            up[2] /= length_up;

            Vector4f fv = Vector4f(f[0], f[1], f[2],0.0f);
            Vector4f s = Vector4f::cross3(fv, Vector4f(up[0], up[1], up[2],0.0f));
            Vector4f u = Vector4f::cross3(s, fv);

            Matrix4f m = null();
            m.matrix[0+0*4] =  s.x;
            m.matrix[0+1*4] =  s.y;
            m.matrix[0+2*4] =  s.z;
            m.matrix[1+0*4] =  u.x;
            m.matrix[1+1*4] =  u.y;
            m.matrix[1+2*4] =  u.z;
            m.matrix[2+0*4] =  -f[0];
            m.matrix[2+1*4] =  -f[1];
            m.matrix[2+2*4] =  -f[2];
            m.matrix[3+3*4] = 1;
            m.multiply(translate( -eye_x,  -eye_y,  -eye_z));
            return m;
    }

    inline Matrix4f Matrix4f::multiply(const Matrix4f &matrix_1, const Matrix4f &matrix_2)
    {
            Matrix4f dest(matrix_2);
            dest.multiply(matrix_1);
            return dest;
    }

    inline Matrix4f Matrix4f::add(const Matrix4f &matrix_1, const Matrix4f &matrix_2)
    {
            Matrix4f dest(matrix_2);
            dest.add(matrix_1);
            return dest;
    }

    inline Matrix4f Matrix4f::subtract(const Matrix4f &matrix_1, const Matrix4f &matrix_2)
    {
            Matrix4f dest(matrix_2);
            dest.subtract(matrix_1);
            return dest;
    }
    inline Matrix4f Matrix4f::adjoint(const Matrix4f &matrix)
    {
            Matrix4f dest(matrix);
            dest.adjoint();
            return dest;
    }

    inline Matrix4f Matrix4f::inverse(const Matrix4f &matrix)
    {
            Matrix4f dest(matrix);
            dest.inverse();
            return dest;
    }

    inline Matrix4f Matrix4f::transpose(const Matrix4f &matrix)
    {
            Matrix4f dest(matrix);
            dest.transpose();
            return dest;
    }

    /////////////////////////////////////////////////////////////////////////////
    // Matrix4f attributes:

    /////////////////////////////////////////////////////////////////////////////
    // Matrix4f operators:



    /////////////////////////////////////////////////////////////////////////////
    // Matrix4f operations:


    inline Matrix4f &Matrix4f::scale_self(float x, float y, float z)
    {
            matrix[0+4*0] *= x;
            matrix[0+4*1] *= y;
            matrix[0+4*2] *= z;

            matrix[1+4*0] *= x;
            matrix[1+4*1] *= y;
            matrix[1+4*2] *= z;

            matrix[2+4*0] *= x;
            matrix[2+4*1] *= y;
            matrix[2+4*2] *= z;

            matrix[3+4*0] *= x;
            matrix[3+4*1] *= y;
            matrix[3+4*2] *= z;
            return *this;
    }

    inline Matrix4f &Matrix4f::translate_self(float x, float y, float z)
    {
            float translate_value_1 = (matrix[0+4*0] * x) + (matrix[0+4*1] * y) + (matrix[0+4*2] * z) + matrix[0+4*3];
            float translate_value_2 = (matrix[1+4*0] * x) + (matrix[1+4*1] * y) + (matrix[1+4*2] * z) + matrix[1+4*3];
            float translate_value_3 = (matrix[2+4*0] * x) + (matrix[2+4*1] * y) + (matrix[2+4*2] * z) + matrix[2+4*3];
            float translate_value_4 = (matrix[3+4*0] * x) + (matrix[3+4*1] * y) + (matrix[3+4*2] * z) + matrix[3+4*3];

            matrix[0+4*3] = translate_value_1;
            matrix[1+4*3] = translate_value_2;
            matrix[2+4*3] = translate_value_3;
            matrix[3+4*3] = translate_value_4;
            return *this;
    }


    inline Matrix4f &Matrix4f::add(const Matrix4f &add_matrix)
    {
            matrix[(0*4) + 0] += add_matrix.matrix[(0*4) + 0];
            matrix[(0*4) + 1] += add_matrix.matrix[(0*4) + 1];
            matrix[(0*4) + 2] += add_matrix.matrix[(0*4) + 2];
            matrix[(0*4) + 3] += add_matrix.matrix[(0*4) + 3];
            matrix[(1*4) + 0] += add_matrix.matrix[(1*4) + 0];
            matrix[(1*4) + 1] += add_matrix.matrix[(1*4) + 1];
            matrix[(1*4) + 2] += add_matrix.matrix[(1*4) + 2];
            matrix[(1*4) + 3] += add_matrix.matrix[(1*4) + 3];
            matrix[(2*4) + 0] += add_matrix.matrix[(2*4) + 0];
            matrix[(2*4) + 1] += add_matrix.matrix[(2*4) + 1];
            matrix[(2*4) + 2] += add_matrix.matrix[(2*4) + 2];
            matrix[(2*4) + 3] += add_matrix.matrix[(2*4) + 3];
            matrix[(3*4) + 0] += add_matrix.matrix[(3*4) + 0];
            matrix[(3*4) + 1] += add_matrix.matrix[(3*4) + 1];
            matrix[(3*4) + 2] += add_matrix.matrix[(3*4) + 2];
            matrix[(3*4) + 3] += add_matrix.matrix[(3*4) + 3];
            return *this;
    }

    inline Matrix4f &Matrix4f::subtract(const Matrix4f &sub_matrix)
    {
            matrix[(0*4) + 0] = sub_matrix.matrix[(0*4) + 0] - matrix[(0*4) + 0];
            matrix[(0*4) + 1] = sub_matrix.matrix[(0*4) + 1] - matrix[(0*4) + 1];
            matrix[(0*4) + 2] = sub_matrix.matrix[(0*4) + 2] - matrix[(0*4) + 2];
            matrix[(0*4) + 3] = sub_matrix.matrix[(0*4) + 3] - matrix[(0*4) + 3];
            matrix[(1*4) + 0] = sub_matrix.matrix[(1*4) + 0] - matrix[(1*4) + 0];
            matrix[(1*4) + 1] = sub_matrix.matrix[(1*4) + 1] - matrix[(1*4) + 1];
            matrix[(1*4) + 2] = sub_matrix.matrix[(1*4) + 2] - matrix[(1*4) + 2];
            matrix[(1*4) + 3] = sub_matrix.matrix[(1*4) + 3] - matrix[(1*4) + 3];
            matrix[(2*4) + 0] = sub_matrix.matrix[(2*4) + 0] - matrix[(2*4) + 0];
            matrix[(2*4) + 1] = sub_matrix.matrix[(2*4) + 1] - matrix[(2*4) + 1];
            matrix[(2*4) + 2] = sub_matrix.matrix[(2*4) + 2] - matrix[(2*4) + 2];
            matrix[(2*4) + 3] = sub_matrix.matrix[(2*4) + 3] - matrix[(2*4) + 3];
            matrix[(3*4) + 0] = sub_matrix.matrix[(3*4) + 0] - matrix[(3*4) + 0];
            matrix[(3*4) + 1] = sub_matrix.matrix[(3*4) + 1] - matrix[(3*4) + 1];
            matrix[(3*4) + 2] = sub_matrix.matrix[(3*4) + 2] - matrix[(3*4) + 2];
            matrix[(3*4) + 3] = sub_matrix.matrix[(3*4) + 3] - matrix[(3*4) + 3];
            return *this;
    }

    inline double Matrix4f::det() const
    {
            Matrix3f temp_matrix;
            float a, b, c, d;
            float value;

            temp_matrix = Matrix3f(
                    matrix[1 * 4 + 1], matrix[2 * 4 + 1], matrix[3 * 4 + 1],
                    matrix[1 * 4 + 2], matrix[2 * 4 + 2], matrix[3 * 4 + 2],
                    matrix[1 * 4 + 3], matrix[2 * 4 + 3], matrix[3 * 4 + 3]);
            a = (float)temp_matrix.det();

            temp_matrix = Matrix3f(
                    matrix[1 * 4 + 0], matrix[2 * 4 + 0], matrix[3 * 4 + 0],
                    matrix[1 * 4 + 2], matrix[2 * 4 + 2], matrix[3 * 4 + 2],
                    matrix[1 * 4 + 3], matrix[2 * 4 + 3], matrix[3 * 4 + 3]);
            b = (float)temp_matrix.det();

            temp_matrix = Matrix3f(
                    matrix[1 * 4 + 0], matrix[2 * 4 + 0], matrix[3 * 4 + 0],
                    matrix[1 * 4 + 1], matrix[2 * 4 + 1], matrix[3 * 4 + 1],
                    matrix[1 * 4 + 3], matrix[2 * 4 + 3], matrix[3 * 4 + 3]);
            c = (float)temp_matrix.det();

            temp_matrix = Matrix3f(
                    matrix[1 * 4 + 0], matrix[2 * 4 + 0], matrix[3 * 4 + 0],
                    matrix[1 * 4 + 1], matrix[2 * 4 + 1], matrix[3 * 4 + 1],
                    matrix[1 * 4 + 2], matrix[2 * 4 + 2], matrix[3 * 4 + 2]);
            d = (float)temp_matrix.det();

            value  = matrix[0 * 4 + 0] * a;
            value -= matrix[0 * 4 + 1] * b;
            value += matrix[0 * 4 + 2] * c;
            value -= matrix[0 * 4 + 3] * d;

            return value;
    }

    inline Matrix4f &Matrix4f::adjoint()
    {
            Matrix4f result;
            Matrix3f m;

            m = Matrix3f(
                            matrix[1*4 + 1], matrix[2*4 + 1], matrix[3*4 + 1],
                            matrix[1*4 + 2], matrix[2*4 + 2], matrix[3*4 + 2],
                            matrix[1*4 + 3], matrix[2*4 + 3], matrix[3*4 + 3] );
            result.matrix[ 0 * 4 + 0 ] = (float) m.det();

            m = Matrix3f(
                            matrix[1*4 + 0], matrix[2*4 + 0], matrix[3*4 + 0],
                            matrix[1*4 + 2], matrix[2*4 + 2], matrix[3*4 + 2],
                            matrix[1*4 + 3], matrix[2*4 + 3], matrix[3*4 + 3] );
            result.matrix[ 1 * 4 + 0 ] = (float) -m.det();

            m = Matrix3f(
                            matrix[1*4 + 0], matrix[2*4 + 0], matrix[3*4 + 0],
                            matrix[1*4 + 1], matrix[2*4 + 1], matrix[3*4 + 1],
                            matrix[1*4 + 3], matrix[2*4 + 3], matrix[3*4 + 3] );
            result.matrix[ 2 * 4 + 0 ] = (float)  m.det();

            m = Matrix3f(
                            matrix[1*4 + 0], matrix[2*4 + 0], matrix[3*4 + 0],
                            matrix[1*4 + 1], matrix[2*4 + 1], matrix[3*4 + 1],
                            matrix[1*4 + 2], matrix[2*4 + 2], matrix[3*4 + 2] );
            result.matrix[ 3 * 4 + 0 ] = (float) -m.det();

            m = Matrix3f(
                            matrix[0*4 + 1], matrix[2*4 + 1], matrix[3*4 + 1],
                            matrix[0*4 + 2], matrix[2*4 + 2], matrix[3*4 + 2],
                            matrix[0*4 + 3], matrix[2*4 + 3], matrix[3*4 + 3] );
            result.matrix[ 0 * 4 + 1 ] = (float) -m.det();

            m = Matrix3f(
                            matrix[0*4 + 0], matrix[2*4 + 0], matrix[3*4 + 0],
                            matrix[0*4 + 2], matrix[2*4 + 2], matrix[3*4 + 2],
                            matrix[0*4 + 3], matrix[2*4 + 3], matrix[3*4 + 3] );
            result.matrix[ 1 * 4 + 1 ] = (float)  m.det();

            m = Matrix3f(
                            matrix[0*4 + 0], matrix[2*4 + 0], matrix[3*4 + 0],
                            matrix[0*4 + 1], matrix[2*4 + 1], matrix[3*4 + 1],
                            matrix[0*4 + 3], matrix[2*4 + 3], matrix[3*4 + 3] );
            result.matrix[ 2 * 4 + 1 ] = (float) -m.det();

            m = Matrix3f(
                            matrix[0*4 + 0], matrix[2*4 + 0], matrix[3*4 + 0],
                            matrix[0*4 + 1], matrix[2*4 + 1], matrix[3*4 + 1],
                            matrix[0*4 + 2], matrix[2*4 + 2], matrix[3*4 + 2] );
            result.matrix[ 3 * 4 + 1 ] = (float)  m.det();

            m = Matrix3f(
                            matrix[0*4 + 1], matrix[1*4 + 1], matrix[3*4 + 1],
                            matrix[0*4 + 2], matrix[1*4 + 2], matrix[3*4 + 2],
                            matrix[0*4 + 3], matrix[1*4 + 3], matrix[3*4 + 3] );
            result.matrix[ 0 * 4 + 2 ] = (float)  m.det();

            m = Matrix3f(
                            matrix[0*4 + 0], matrix[1*4 + 0], matrix[3*4 + 0],
                            matrix[0*4 + 2], matrix[1*4 + 2], matrix[3*4 + 2],
                            matrix[0*4 + 3], matrix[1*4 + 3], matrix[3*4 + 3] );
            result.matrix[ 1 * 4 + 2 ] = (float) -m.det();

            m = Matrix3f(
                            matrix[0*4 + 0], matrix[1*4 + 0], matrix[3*4 + 0],
                            matrix[0*4 + 1], matrix[1*4 + 1], matrix[3*4 + 1],
                            matrix[0*4 + 3], matrix[1*4 + 3], matrix[3*4 + 3] );
            result.matrix[ 2 * 4 + 2 ] = (float)  m.det();

            m = Matrix3f(
                            matrix[0*4 + 0], matrix[1*4 + 0], matrix[3*4 + 0],
                            matrix[0*4 + 1], matrix[1*4 + 1], matrix[3*4 + 1],
                            matrix[0*4 + 2], matrix[1*4 + 2], matrix[3*4 + 2] );
            result.matrix[ 3 * 4 + 2 ] = (float) -m.det();

            m = Matrix3f(
                            matrix[0*4 + 1], matrix[1*4 + 1], matrix[2*4 + 1],
                            matrix[0*4 + 2], matrix[1*4 + 2], matrix[2*4 + 2],
                            matrix[0*4 + 3], matrix[1*4 + 3], matrix[2*4 + 3] );
            result.matrix[ 0 * 4 + 3 ] = (float) -m.det();

            m = Matrix3f(
                            matrix[0*4 + 0], matrix[1*4 + 0], matrix[2*4 + 0],
                            matrix[0*4 + 2], matrix[1*4 + 2], matrix[2*4 + 2],
                            matrix[0*4 + 3], matrix[1*4 + 3], matrix[2*4 + 3] );
            result.matrix[ 1 * 4 + 3 ] = (float)  m.det();

            m = Matrix3f(
                            matrix[0*4 + 0], matrix[1*4 + 0], matrix[2*4 + 0],
                            matrix[0*4 + 1], matrix[1*4 + 1], matrix[2*4 + 1],
                            matrix[0*4 + 3], matrix[1*4 + 3], matrix[2*4 + 3] );
            result.matrix[ 2 * 4 + 3 ] = (float) -m.det();

            m = Matrix3f(
                            matrix[0*4 + 0], matrix[1*4 + 0], matrix[2*4 + 0],
                            matrix[0*4 + 1], matrix[1*4 + 1], matrix[2*4 + 1],
                            matrix[0*4 + 2], matrix[1*4 + 2], matrix[2*4 + 2] );
            result.matrix[ 3 * 4 + 3 ] = (float)  m.det();

            *this = result;
            return *this;
    }

    inline Matrix4f &Matrix4f::inverse()
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
                    Matrix4f result = *this;
                    result.adjoint();

                    d=1.0/d;	// Inverse the determinant
                    for (int i=0; i<16; i++)
                    {
                            result.matrix[i] = (float) (result.matrix[i] * d);
                    }

                    *this = result;
            }
            return *this;
    }

    inline Matrix4f &Matrix4f::transpose()
    {
            float original[16];
            for (int cnt=0; cnt<16; cnt++)
                    original[cnt] = matrix[cnt];

            matrix[0] = original[0];
            matrix[1] = original[4];
            matrix[2] = original[8];
            matrix[3] = original[12];
            matrix[4] = original[1];
            matrix[5] = original[5];
            matrix[6] = original[9];
            matrix[7] = original[13];
            matrix[8] = original[2];
            matrix[9] = original[6];
            matrix[10] = original[10];
            matrix[11] = original[14];
            matrix[12] = original[3];
            matrix[13] = original[7];
            matrix[14] = original[11];
            matrix[15] = original[13];

            return *this;
    }

    /////////////////////////////////////////////////////////////////////////////
    // Matrix4f implementation:





    inline Matrix3f::Matrix3f(const Matrix4f &copy)
    {
            matrix[0*3 + 0] = copy.matrix[0*4 + 0];
            matrix[0*3 + 1] = copy.matrix[0*4 + 1];
            matrix[0*3 + 2] = copy.matrix[0*4 + 2];
            matrix[1*3 + 0] = copy.matrix[1*4 + 0];
            matrix[1*3 + 1] = copy.matrix[1*4 + 1];
            matrix[1*3 + 2] = copy.matrix[1*4 + 2];
            matrix[2*3 + 0] = copy.matrix[2*4 + 0];
            matrix[2*3 + 1] = copy.matrix[2*4 + 1];
            matrix[2*3 + 2] = copy.matrix[2*4 + 2];
    }

    inline Matrix3f &Matrix3f::operator =(const Matrix4f &copy)
    {
            matrix[0*3 + 0] = copy.matrix[0*4 + 0];
            matrix[0*3 + 1] = copy.matrix[0*4 + 1];
            matrix[0*3 + 2] = copy.matrix[0*4 + 2];
            matrix[1*3 + 0] = copy.matrix[1*4 + 0];
            matrix[1*3 + 1] = copy.matrix[1*4 + 1];
            matrix[1*3 + 2] = copy.matrix[1*4 + 2];
            matrix[2*3 + 0] = copy.matrix[2*4 + 0];
            matrix[2*3 + 1] = copy.matrix[2*4 + 1];
            matrix[2*3 + 2] = copy.matrix[2*4 + 2];
            return *this;
    }




    inline Vector4f operator * (const Vector4f& v, const Matrix4f& m)
    {
            return Vector4f(
                    m.matrix[0*4+0]*v.x + m.matrix[0*4+1]*v.y + m.matrix[0*4+2]*v.z + m.matrix[0*4+3]*v.w,
                    m.matrix[1*4+0]*v.x + m.matrix[1*4+1]*v.y + m.matrix[1*4+2]*v.z + m.matrix[1*4+3]*v.w,
                    m.matrix[2*4+0]*v.x + m.matrix[2*4+1]*v.y + m.matrix[2*4+2]*v.z + m.matrix[2*4+3]*v.w,
                    m.matrix[3*4+0]*v.x + m.matrix[3*4+1]*v.y + m.matrix[3*4+2]*v.z + m.matrix[3*4+3]*v.w);
    }

    inline Vector4f operator * (const Matrix4f& m, const Vector4f& v)
    {
            return Vector4f(
                    m.matrix[0*4+0]*v.x + m.matrix[1*4+0]*v.y + m.matrix[2*4+0]*v.z + m.matrix[3*4+0]*v.w,
                    m.matrix[0*4+1]*v.x + m.matrix[1*4+1]*v.y + m.matrix[2*4+1]*v.z + m.matrix[3*4+1]*v.w,
                    m.matrix[0*4+2]*v.x + m.matrix[1*4+2]*v.y + m.matrix[2*4+2]*v.z + m.matrix[3*4+2]*v.w,
                    m.matrix[0*4+3]*v.x + m.matrix[1*4+3]*v.y + m.matrix[2*4+3]*v.z + m.matrix[3*4+3]*v.w);
    }



}

#endif /*SB_MATRIX2_H_INCLUDED*/
