#ifndef SB_VECTOR2_H_INCLUDED
#define SB_VECTOR2_H_INCLUDED

#include "sb_math.h"

namespace Sandbox {


    struct Vector2f;

    struct Vector3f;

    struct Vector4f;

    struct Matrix2f;

    struct Matrix3f;

    struct Matrix4f;



    /// 2D vector
    struct Vector2f
    {
   
            float x;
            float y;

            Vector2f() : x(0.0f), y(0.0f) { }
            explicit Vector2f(const Vector3f &copy);
            explicit Vector2f(const Vector4f &copy);
            Vector2f(float p1, float p2 ) : x(p1), y(p2) { }

            Vector2f(const Vector2f &copy);

            /// \brief Normalizes a vector
            ///
            /// Operates in the native datatype
            ///
            /// \param vector = Vector to use
            /// \return normalized vector
            static Vector2f normalize(const Vector2f& vector);

            /// \brief Dot products a vector with an other vector.
            ///
            /// Operates in the native datatype
            ///
            /// \param vector_1 = First vector used for the dot product.
            /// \param vector_2 = Second vector used for the dot product.
            /// \return The resulting dot product of the two vectors.
            static float dot(const Vector2f& vector_1, const Vector2f& vector_2) { return vector_1.x*vector_2.x + vector_1.y*vector_2.y; }

            /// \brief Rounds all components on a vector
            ///
            /// Uses Asymmetric Arithmetic Rounding
            ///
            /// \param vector = Vector to use
            /// \return The rounded vector
            static Vector2f round(const Vector2f& vector);

            /// \brief Rotate a vector around another point.
            ///
            /// \param vector = Vector to use
            /// \param hotspot The point around which to rotate.
            /// \param angle = Angle to rotate.
            static Vector2f rotate(const Vector2f& vector, const Vector2f& hotspot, float angle);


    /// \name Attributes
    /// \{

  
            /// \brief Returns the length (magnitude) of this vector.
            ///
            /// Operates in the native datatype
            ///
            /// \return the length of the vector
            float length() const;

            /// \brief Normalizes this vector
            ///
            /// Operates in the native datatype
            ///
            /// \return reference to this object
            Vector2f &normalize();

            /// \brief Return normalized vector with some dir
            ///
            Vector2f unit() const { Vector2f res(*this); res.normalize(); return res; }

            /// \brief Return normal for this vector
            ///
            Vector2f normal() const { return Vector2f(-y,x); }

            /// \brief Dot products this vector with an other vector.
            ///
            /// Operates in the native datatype
            ///
            /// \param vector Second vector used for the dot product.
            ///
            /// \return The resulting dot product of the two vectors.
            float dot(const Vector2f& vector) const {return x*vector.x + y*vector.y;}

            /// \brief Calculate the angle between this vector and an other vector.
            ///
            /// \param vector = Second vector used to calculate angle.
            ///
            /// \return The angle between the two vectors.
            float angle(const Vector2f& vector) const;

            /// \brief Calculate the distance between this vector and an other vector.
            ///
            /// \param vector = Second vector used to calculate distance.
            ///
            /// \return The distance between the two vectors.
            float distance(const Vector2f& vector) const;

            /// \brief Rounds all components of this vector
            ///
            /// Uses Asymmetric Arithmetic Rounding
            ///
            /// \return reference to this object
            Vector2f &round();

            /// \brief Rotate this vector around another point.
            ///
            /// \param hotspot The point around which to rotate.
            /// \param angle = Angle to rotate.
            ///
            /// \return reference to this object
            Vector2f &rotate(const Vector2f& hotspot, float angle);

			/// \brief Rotate this vector.
			///
			/// \param angle = Angle to rotate.
			///
			/// \return reference to this object
			Vector2f &rotate(float angle);
    
    /// \}
    /// \name Operators
    /// \{

 
            /// \brief += operator.
            void operator += (const Vector2f& vector) { x+= vector.x; y+= vector.y; }

            /// \brief += operator.
            void operator += ( float value) { x+= value; y+= value; }

            /// \brief + operator.
            Vector2f operator + (const Vector2f& vector) const {return Vector2f(vector.x + x, vector.y + y);}

            /// \brief + operator.
            Vector2f operator + (float value) const {return Vector2f(value + x, value + y);}

            /// \brief -= operator.
            void operator -= (const Vector2f& vector) { x-= vector.x; y-= vector.y; }

            /// \brief -= operator.
            void operator -= ( float value) { x-= value; y-= value; }

            /// \brief - operator.
            Vector2f operator - (const Vector2f& vector) const {return Vector2f(x - vector.x, y - vector.y);}

            /// \brief - operator.
            Vector2f operator - (float value) const {return Vector2f(x - value, y - value);}

            /// \brief - operator.
            Vector2f operator - () const {return Vector2f(-x , -y);}

            /// \brief *= operator.
            void operator *= (const Vector2f& vector) { x*= vector.x; y*= vector.y; }

            /// \brief *= operator.
            void operator *= ( float value) { x*= value; y*= value; }

            /// \brief * operator.
            Vector2f operator * (const Vector2f& vector) const {return Vector2f(vector.x * x, vector.y * y);}

            /// \brief * operator.
            Vector2f operator * (float value) const {return Vector2f(value * x, value * y);}

            /// \brief /= operator.
            void operator /= (const Vector2f& vector) { x/= vector.x; y/= vector.y; }

            /// \brief /= operator.
            void operator /= ( float value) { x/= value; y/= value; }

            /// \brief / operator.
            Vector2f operator / (const Vector2f& vector) const {return Vector2f(x / vector.x, y / vector.y);}

            /// \brief / operator.
            Vector2f operator / (float value) const {return Vector2f(x / value, y / value);}

            /// \brief = operator.
            Vector2f &operator = (const Vector2f& vector) { x = vector.x; y = vector.y; return *this; }

            /// \brief == operator.
            bool operator == (const Vector2f& vector) const {return ((x == vector.x) && (y == vector.y));}

            /// \brief != operator.
            bool operator != (const Vector2f& vector) const {return ((x != vector.x) || (y != vector.y));}
    /// \}
    };


    //////////////////////////////////////////////////////////////////////////


    inline Vector2f::Vector2f(const Vector2f &copy) { x = copy.x; y = copy.y; }




    //////////////////////////////////////////////////////////////////////////


    /// \}

























    

    inline float Vector2f::length() const {return sqrt(x*x+y*y);}

    inline Vector2f &Vector2f::normalize()
    {
            float f = length();
            if (f!=0.0f)
            {
                    x /= f;
                    y /= f;
            }
            return *this;
    }

    inline Vector2f Vector2f::normalize(const Vector2f& vector)
    {
            Vector2f dest(vector);
            dest.normalize();
            return dest;
    }

    inline float Vector2f::angle(const Vector2f& v) const
    {
		return acos(dot(v)/(length()*v.length()));
    }


    inline Vector2f &Vector2f::round()
    {
            x =  floor(x+0.5f);
            y =  floor(y+0.5f);
            return *this;
    }


    inline float Vector2f::distance(const Vector2f &vector) const
    {
            float value_x, value_y;
            value_x = x - vector.x;
            value_y = y - vector.y;
            return float(sqrt(value_x*value_x + value_y*value_y));
    }


    inline Vector2f Vector2f::round(const Vector2f& vector)
    {
            Vector2f dest(vector);
            dest.round();
            return dest;
    }

    inline Vector2f &Vector2f::rotate(const Vector2f& hotspot, float angle_)
    {
            // Move the hotspot to 0,0
            Vector2f r(x - hotspot.x, y - hotspot.y);

            float sin_angle = sin(angle_);
            float cos_angle = cos(angle_);

            float dest_x =  r.x * cos_angle - r.y * sin_angle;
            float dest_y =  r.x * sin_angle + r.y * cos_angle;

            x = dest_x + hotspot.x;
            y = dest_y + hotspot.y;
            return *this;
    }
	
	inline Vector2f &Vector2f::rotate(float angle_)
    {
		float sin_angle = sin(angle_);
		float cos_angle = cos(angle_);
		
		float dest_x =  x * cos_angle - y * sin_angle;
		float dest_y =  x * sin_angle + y * cos_angle;
		
		x = dest_x;
		y = dest_y;
		return *this;
    }
	

    inline Vector2f Vector2f::rotate(const Vector2f& vector, const Vector2f& hotspot, float angle_)
    {
            Vector2f dest(vector);
            dest.rotate(hotspot, angle_);
            return dest;
    }

   


}

#include <vector>

namespace Sandbox {
	/// @todo remove
	class Vector2fList : public std::vector<Vector2f> {
	public:
		void add(const Vector2f& v) { push_back(v);}
	};
	
}


#endif /*SB_VECTOR2_H_INCLUDED*/
