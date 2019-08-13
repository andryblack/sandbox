#ifndef SB_VECTOR4_H_INCLUDED
#define SB_VECTOR4_H_INCLUDED


#include "sb_vector2.h"
#include "sb_vector3.h"

namespace Sandbox {


    struct Vector2f;

    struct Vector3f;

    struct Vector4f;

    union Matrix2f;

    struct Matrix3f;

    struct Matrix4f;


   
    ///  4D vector
    struct Vector4f
    {
   
            float x;
            float y;
            float z;
            float w;

            Vector4f() : x(0), y(0), z(0), w(0) { }
            explicit Vector4f(const Vector2f &copy) { x = copy.x; y = copy.y; z = 0; w = 0; }
            explicit Vector4f(const Vector3f &copy) { x = copy.x; y = copy.y; z = copy.z; w = 0; }
            Vector4f( float p1, float p2 , float p3 , float p4 ) : x(p1), y(p2), z(p3), w(p4) { }

            /// \brief Normalizes a vector (not taking into account the w ordinate)
            ///
            /// Operates in the native datatype
            /// \param vector = The vector to use
            /// \return the normalized vector
            static Vector4f normalize3(const Vector4f &vector);

            /// \brief Normalizes a vector (taking into account the w ordinate)
            ///
            /// Operates in the native datatype
            /// \param vector = The vector to use
            /// \return the normalized vector
            static Vector4f normalize4(const Vector4f &vector);

            /// \brief Dot products between two vectors (not taking into account the w ordinate).
            ///
            /// Operates in the native datatype
            ///
            /// \param vector1 First vector used for the dot product.
            /// \param vector2 Second vector used for the dot product.
            /// \return The resulting dot product of the two vectors.
            static float dot3(const Vector4f& vector1, const Vector4f& vector2) { return vector1.x*vector2.x + vector1.y*vector2.y + vector1.z*vector2.z; }

            /// \brief Dot products between two vectors (taking into account the w ordinate).
            ///
            /// Operates in the native datatype
            ///
            /// \param vector1 First vector used for the dot product.
            /// \param vector2 Second vector used for the dot product.
            /// \return The resulting dot product of the two vectors.
            static float dot4(const Vector4f& vector1, const Vector4f& vector2) { return vector1.x*vector2.x + vector1.y*vector2.y + vector1.z*vector2.z + vector1.w*vector2.w; }

            /// \brief Calculate the cross product between two vectors (not taking into account the w ordinate).
            ///
            /// \vector1 = The first vector
            /// \vector2 = The second vector
            /// \return = Cross product
            static Vector4f cross3(const Vector4f& vector1, const Vector4f& vector2);

            /// \brief Rotate a vector around an axis. Same as glRotate[f|d](angle, a);
            ///
            /// Not taking into account the w ordinate\n
            ///  The w ordinate is not modified
            ///
            /// \param vector = The vector to use
            /// \param angle Angle to rotate
            /// \param axis Rotation axis.
            /// \return The rotated vector
            static Vector4f rotate3(const Vector4f& vector, float angle, const Vector4f& axis);

            /// \brief Rounds all components on a vector.
            ///
            /// Includes the w ordinate\n
            /// Uses Asymmetric Arithmetic Rounding
            /// \param vector = The vector to use
            /// \return The rounded vector
            static Vector4f round(const Vector4f& vector);

  
    public:
            /// \brief Returns the length (magnitude) of this vector (not taking into account the w ordinate).
            ///
            /// Operates in the native datatype
            ///
            /// \return the length of the vector */
            float length3() const;

            /// \brief Returns the length (magnitude) of this vector (taking into account the w ordinate).
            ///
            /// Operates in the native datatype
            ///
            /// \return the length of the vector */
            float length4() const;

            /// \brief Normalizes this vector (not taking into account the w ordinate)
            ///
            /// Operates in the native datatype
            /// \return reference to this object
            Vector4f &normalize3();

            /// \brief Normalizes this vector (taking into account the w ordinate)
            ///
            /// Operates in the native datatype
            /// \return reference to this object
            Vector4f &normalize4();

            /// \brief Dot products this vector with an other vector (not taking into account the w ordinate).
            ///
            /// Operates in the native datatype
            ///
            /// \param vector Second vector used for the dot product.
            /// \return The resulting dot product of the two vectors.
            float dot3(const Vector4f& vector) const {return x*vector.x + y*vector.y + z*vector.z;}

            /// \brief Dot products this vector with an other vector (taking into account the w ordinate).
            ///
            /// Operates in the native datatype
            ///
            /// \param vector Second vector used for the dot product.
            /// \return The resulting dot product of the two vectors.
            float dot4(const Vector4f& vector) const {return x*vector.x + y*vector.y + z*vector.z + w*vector.w;}

            /// \brief Calculate the angle between this vector and an other vector (not taking into account the w ordinate).
            ///
            /// \param vector = Second vector used to calculate angle.
            ///
            /// \return The angle between the two vectors.
            float angle3(const Vector4f& vector) const;

            /// \brief Calculate the distance between this vector and an other vector (not taking into account the w ordinate).
            ///
            /// \param vector = Second vector used to calculate distance.
            ///
            /// \return The distance between the two vectors.
            float distance3(const Vector4f& vector) const;

            /// \brief Calculate the distance between this vector and an other vector (taking into account the w ordinate).
            ///
            /// \param vector = Second vector used to calculate distance.
            ///
            /// \return The distance between the two vectors.
            float distance4(const Vector4f& vector) const;

            /// \brief Calculate the cross product between this vector and an other vector (not taking into account the w ordinate).
            ///
            /// Operates in the native datatype
            ///
            /// \param vector Second vector used to perform the calculation.
            /// \return reference to this object
            Vector4f &cross3(const Vector4f& vector);

            /// \brief Rotate this vector around an axis. Same as glRotate[f|d](angle, a);
            ///
            /// Not taking into account the w ordinate\n
            ///  The w ordinate is not modified
            ///
            /// \param angle Angle to rotate
            /// \param axis Rotation axis.
            /// \return reference to this object
            Vector4f &rotate3(float angle, const Vector4f& axis);

            /// \brief Rounds all components on this vector.
            ///
            /// Includes the w ordinate\n
            /// Uses Asymmetric Arithmetic Rounding
            /// \return reference to this object
            Vector4f &round();

  
    public:
            const float &operator[](unsigned int i) const { return ((float *) this)[i]; }
            float &operator[](unsigned int i) { return ((float *) this)[i]; }

            /// \brief += operator.
            void operator += (const Vector4f& vector) { x+= vector.x; y+= vector.y; z+= vector.z; w+= vector.w; }

            /// \brief += operator.
            void operator += ( float value) { x+= value; y+= value; z+= value; w+= value; }

            /// \brief + operator.
            Vector4f operator + (const Vector4f& vector) const {return Vector4f(vector.x + x, vector.y + y, vector.z + z, vector.w + w);}

            /// \brief + operator.
            Vector4f operator + (float value) const {return Vector4f(value + x, value + y, value + z, value + w);}

            /// \brief -= operator.
            void operator -= (const Vector4f& vector) { x-= vector.x; y-= vector.y; z-= vector.z; w-= vector.w; }

            /// \brief -= operator.
            void operator -= ( float value) { x-= value; y-= value; z-= value; w-= value; }

            /// \brief - operator.
            Vector4f operator - (const Vector4f& vector) const {return Vector4f(x - vector.x, y - vector.y, z - vector.z, w - vector.w);}

            /// \brief - operator.
            Vector4f operator - (float value) const {return Vector4f(x - value, y - value, z - value, w - value);}

            /// \brief *= operator.
            void operator *= (const Vector4f& vector) { x*= vector.x; y*= vector.y; z*= vector.z; w*= vector.w; }

            /// \brief *= operator.
            void operator *= ( float value) { x*= value; y*= value; z*= value; w*= value; }

            /// \brief * operator.
            Vector4f operator * (const Vector4f& vector) const {return Vector4f(vector.x * x, vector.y * y, vector.z * z, vector.w * w);}

            /// \brief * operator.
            Vector4f operator * (float value) const {return Vector4f(value * x, value * y, value * z, value * w);}

            /// \brief /= operator.
            void operator /= (const Vector4f& vector) { x/= vector.x; y/= vector.y; z/= vector.z; w/= vector.w; }

            /// \brief /= operator.
            void operator /= ( float value) { x/= value; y/= value; z/= value; w/= value; }

            /// \brief / operator.
            Vector4f operator / (const Vector4f& vector) const {return Vector4f(x / vector.x, y / vector.y, z / vector.z, w / vector.w);}

            /// \brief / operator.
            Vector4f operator / (float value) const {return Vector4f(x / value, y / value, z / value, w / value);}

            /// \brief = operator.
            Vector4f &operator = (const Vector4f& vector) { x = vector.x; y = vector.y; z = vector.z; w = vector.w; return *this; }

            /// \brief == operator.
            bool operator == (const Vector4f& vector) const {return ((x == vector.x) && (y == vector.y) && (z == vector.z) && (w == vector.w));}

            /// \brief != operator.
            bool operator != (const Vector4f& vector) const {return ((x != vector.x) || (y != vector.y) || (z != vector.z) || (w != vector.w));}
    };



   















    inline float Vector4f::length3() const {return std::sqrt(x*x+y*y+z*z);}


    inline float Vector4f::length4() const {return std::sqrt(x*x+y*y+z*z+w*w);}

    inline Vector4f &Vector4f::normalize3()
    {
            float f = length3();
            if (f!=0)
            {
                    x /= f;
                    y /= f;
                    z /= f;
            }
            return *this;
    }

    inline Vector4f Vector4f::normalize3(const Vector4f& vector)
    {
            Vector4f dest(vector);
            dest.normalize3();
            return dest;
    }

    inline Vector4f &Vector4f::normalize4()
    {
            float f = length4();
            if (f!=0)
            {
                    x /= f;
                    y /= f;
                    z /= f;
                    w /= f;
            }
            return *this;
    }

    inline Vector4f Vector4f::normalize4(const Vector4f& vector)
    {
            Vector4f dest(vector);
            dest.normalize4();
            return dest;
    }

    inline float Vector4f::angle3(const Vector4f& v) const
    {
		return acos(dot3(v)/(length3()*v.length3()));
    }

    inline float Vector4f::distance3(const Vector4f &vector) const
    {
            float value_x, value_y, value_z;
            value_x = x - vector.x;
            value_y = y - vector.y;
            value_z = z - vector.z;
		return sqrt(value_x*value_x + value_y*value_y+value_z*value_z);
    }




    inline float Vector4f::distance4(const Vector4f &vector) const
    {
            float value_x, value_y, value_z, value_w;
            value_x = x - vector.x;
            value_y = y - vector.y;
            value_z = z - vector.z;
            value_w = w - vector.w;
		return sqrt(value_x*value_x + value_y*value_y+value_z*value_z+value_w*value_w);
    }

    inline Vector4f Vector4f::cross3(const Vector4f& v1, const Vector4f& v2)
    {
            Vector4f tmp = Vector4f(v1.y * v2.z - v1.z * v2.y,
                                      v1.z * v2.x - v1.x * v2.z,
                                      v1.x * v2.y - v1.y * v2.x, 0);
            return tmp;
    }

    inline Vector4f &Vector4f::cross3(const Vector4f& v)
    {
            *this = cross3(*this, v);
            return *this;
    }

    // Same as glRotate[f/d](angle, a);
    inline Vector4f &Vector4f::rotate3(float angle_, const Vector4f& a)
    {
            Vector4f tmp = Vector4f();

		float s = sin(angle_);
		float c = cos(angle_);

            tmp.x =  (x*(a.x*a.x*(1.0f-c)+c)     + y*(a.x*a.y*(1.0f-c)-a.z*s) + z*(a.x*a.z*(1.0f-c)+a.y*s));
            tmp.y =  (x*(a.y*a.x*(1.0f-c)+a.z*s) + y*(a.y*a.y*(1.0f-c)+c)     + z*(a.y*a.z*(1.0f-c)-a.x*s));
            tmp.z =  (x*(a.x*a.z*(1.0f-c)-a.y*s) + y*(a.y*a.z*(1.0f-c)+a.x*s) + z*(a.z*a.z*(1.0f-c)+c));
            *this = tmp;
            return *this;
    }

    inline Vector4f Vector4f::rotate3(const Vector4f& vector, float angle_, const Vector4f& a)
    {
            Vector4f dest(vector);
            dest.rotate3(angle_, a);
            return dest;
    }

    inline Vector4f &Vector4f::round()
    {
            x = floor(x+0.5f);
            y = floor(y+0.5f);
            z = floor(z+0.5f);
            w = floor(w+0.5f);
            return *this;
    }

    inline Vector4f Vector4f::round(const Vector4f& vector)
    {
            Vector4f dest(vector);
            dest.round();
            return dest;
    }


    inline Vector3f::Vector3f(const Vector4f &copy) {
          x = copy.x; y = copy.y; z = copy.z;
    }

}

#endif /*SB_VECTOR4_H_INCLUDED*/
