#ifndef SB_VECTOR3_H_INCLUDED
#define SB_VECTOR3_H_INCLUDED


#include "sb_vector2.h"

namespace Sandbox {


    struct Vector2f;

    struct Vector3f;

    struct Vector4f;

    struct Matrix2f;

    struct Matrix3f;

    struct Matrix4f;


  
    /// \brief 3D vector
    struct Vector3f
    {
  
            float x;
            float y;
            float z;

            Vector3f() : x(0), y(0), z(0) { }
            explicit Vector3f(const Vector2f &copy) { x = copy.x; y = copy.y; z = 0; }
            explicit Vector3f(const Vector4f &copy);

            Vector3f(const Vector3f &copy);

            Vector3f(float p1, float p2 , float p3) : x(p1), y(p2), z(p3) { }

            /// \brief Normalizes a vector
            ///
            /// \param vector = Vector to use
            ///
            /// Operates in the native datatype
            static Vector3f normalize(const Vector3f& vector);

            /// \brief Dot products between two vectors.
            ///
            /// \return The dot product
            static float dot(const Vector3f& vector1, const Vector3f& vector2) { return vector1.x*vector2.x + vector1.y*vector2.y + vector1.z*vector2.z; }

            /// \brief Calculate the cross product between two vectors.
            ///
            /// \param vector1 = The first vector
            /// \param vector2 = The second vector
            /// \return The cross product
            static Vector3f cross(const Vector3f& vector1, const Vector3f& vector2);

            /// \brief Rotate a vector around an axis. Same as glRotate[f|d](angle, a);
            ///
            /// \param vector = Vector to use
            /// \param angle = Angle to rotate.
            /// \param axis = Rotation axis.
            /// \return The rotated vector
            static Vector3f rotate(const Vector3f& vector, float angle, const Vector3f& axis);

            /// \brief Rounds all components on a vector
            ///
            /// Uses Asymmetric Arithmetic Rounding
            /// \param vector = Vector to use
            /// \return The rounded vector
            static Vector3f round(const Vector3f& vector);

			/// \brief Returns the length (magnitude) of this vector.
            ///
            /// Operates in the native datatype
            /// \return the length of the vector
            float length() const;

            /// \brief Normalizes this vector
            ///
            /// Operates in the native datatype
            /// \return reference to this object
            Vector3f &normalize() {
                float ilen = 1.0f / length();
                x*=ilen;
                y*=ilen;
                z*=ilen;
                return *this;
            }

		
            /// \brief Dot products this vector with an other vector.
            ///
            /// Operates in the native datatype
            ///
            /// \param vector Second vector used for the dot product.
            /// \return The resulting dot product of the two vectors.
            float dot(const Vector3f& vector) const { return x*vector.x + y*vector.y + z*vector.z; }

            /// \brief Calculate the angle between this vector and an other vector.
            ///
            /// \param vector Second vector used to calculate angle.
            ///
            /// \return The angle between the two vectors.
            float angle(const Vector3f& vector) const;

            /// \brief Calculate the distance between this vector and an other vector.
            ///
            /// \param vector = Second vector used to calculate distance.
            ///
            /// \return The distance between the two vectors.
            float distance(const Vector3f& vector) const;

            /// \brief Calculate the cross product between this vector and an other vector.
            ///
            /// Operates in the native datatype
            /// \param vector = Second vector used to perform the calculation.
            /// \return reference to this object
            Vector3f &cross(const Vector3f& vector);

            /// \brief Rotate this vector around an axis. Same as glRotate[f|d](angle, a);
            ///
            /// \param angle Angle to rotate.
            /// \param axis Rotation axis.
            /// \return reference to this object
            Vector3f &rotate(float angle, const Vector3f& axis);

            /// \brief Rounds all components on this vector
            ///
            //// Uses Asymmetric Arithmetic Rounding
            /// \return reference to this object
            Vector3f &round();

    
            /// \brief += operator.
            void operator += (const Vector3f& vector) { x+= vector.x; y+= vector.y; z+= vector.z; }

            /// \brief += operator.
            void operator += ( float value) { x+= value; y+= value; z+= value; }

            /// \brief + operator.
            Vector3f operator + (const Vector3f& vector) const {return Vector3f(vector.x + x, vector.y + y, vector.z + z);}

            /// \brief + operator.
            Vector3f operator + (float value) const {return Vector3f(value + x, value + y, value + z);}

            /// \brief -= operator.
            void operator -= (const Vector3f& vector) { x-= vector.x; y-= vector.y; z-= vector.z; }

            /// \brief -= operator.
            void operator -= ( float value) { x-= value; y-= value; z-= value; }

            /// \brief - operator.
            Vector3f operator - (const Vector3f& vector) const {return Vector3f(x - vector.x, y - vector.y, z - vector.z);}

            /// \brief - operator.
            Vector3f operator - (float value) const {return Vector3f(x - value, y - value, z - value);}

            /// \brief *= operator.
            void operator *= (const Vector3f& vector) { x*= vector.x; y*= vector.y; z*= vector.z; }

            /// \brief *= operator.
            void operator *= ( float value) { x*= value; y*= value; z*= value; }

            /// \brief * operator.
            Vector3f operator * (const Vector3f& vector) const {return Vector3f(vector.x * x, vector.y * y, vector.z * z);}

            /// \brief * operator.
            Vector3f operator * (float value) const {return Vector3f(value * x, value * y, value * z);}

            /// \brief /= operator.
            void operator /= (const Vector3f& vector) { x/= vector.x; y/= vector.y; z/= vector.z; }

            /// \brief /= operator.
            void operator /= ( float value) { x/= value; y/= value; z/= value; }

            /// \brief / operator.
            Vector3f operator / (const Vector3f& vector) const {return Vector3f(x / vector.x, y / vector.y, z / vector.z);}

            /// \brief / operator.
            Vector3f operator / (float value) const {return Vector3f(x / value, y / value, z / value);}

            /// \brief = operator.
            Vector3f &operator = (const Vector3f& vector) { x = vector.x; y = vector.y; z = vector.z; return *this; }

            /// \brief == operator.
            bool operator == (const Vector3f& vector) const {return ((x == vector.x) && (y == vector.y) && (z == vector.z));}

            /// \brief != operator.
            bool operator != (const Vector3f& vector) const {return ((x != vector.x) || (y != vector.y) || (z != vector.z));}
    };

    

    inline Vector3f::Vector3f(const Vector3f &copy) { x = (float) copy.x; y = (float) copy.y; z = (float) copy.z; }

















    inline float Vector3f::length() const {return std::sqrt(x*x+y*y+z*z);}

    
    inline Vector3f Vector3f::normalize(const Vector3f& vector)
    {
            Vector3f dest(vector);
            dest.normalize();
            return dest;
    }

    inline float Vector3f::angle(const Vector3f& v) const
    {
            return acos(dot(v)/(length()*v.length()));
    }

    inline Vector3f Vector3f::cross(const Vector3f& v1, const Vector3f& v2)
    {
            Vector3f tmp = Vector3f(v1.y * v2.z - v1.z * v2.y,
                                      v1.z * v2.x - v1.x * v2.z,
                                      v1.x * v2.y - v1.y * v2.x);
            return tmp;
    }

    inline Vector3f &Vector3f::cross(const Vector3f& v)
    {
            cross(*this, v);
            return *this;
    }

    inline float Vector3f::distance(const Vector3f &vector) const
    {
            float value_x, value_y, value_z;
            value_x = x - vector.x;
            value_y = y - vector.y;
            value_z = z - vector.z;
		return std::sqrt(value_x*value_x + value_y*value_y+value_z*value_z);
    }


    // Same as glRotate[f|d](angle, a);
    inline Vector3f &Vector3f::rotate(float angle_, const Vector3f& a)
    {
            Vector3f tmp = Vector3f();

			float s = sin(angle_);
			float c = cos(angle_);

            tmp.x =  (x*(a.x*a.x*(1.0f-c)+c)     + y*(a.x*a.y*(1.0f-c)-a.z*s) + z*(a.x*a.z*(1.0f-c)+a.y*s));
            tmp.y =  (x*(a.y*a.x*(1.0f-c)+a.z*s) + y*(a.y*a.y*(1.0f-c)+c)     + z*(a.y*a.z*(1.0f-c)-a.x*s));
            tmp.z =  (x*(a.x*a.z*(1.0f-c)-a.y*s) + y*(a.y*a.z*(1.0f-c)+a.x*s) + z*(a.z*a.z*(1.0f-c)+c));
            *this = tmp;
            return *this;
    }

    inline Vector3f Vector3f::rotate(const Vector3f &vector, float angle_, const Vector3f& a)
    {
            Vector3f dest(vector);
            dest.rotate(angle_, a);
            return dest;
    }

    inline Vector3f &Vector3f::round()
    {
            x =  floor(x+0.5f);
            y =  floor(y+0.5f);
            z =  floor(z+0.5f);
            return *this;
    }

    inline Vector3f Vector3f::round(const Vector3f& vector)
    {
            Vector3f dest(vector);
            dest.round();
            return dest;
    }



   

}

#endif /*SB_VECTOR3_H_INCLUDED*/
