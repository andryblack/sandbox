/*
 *  sb_transform2d.h
 *  SR
 *
 *  Created by Андрей Куницын on 07.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_TRANSFORM2D_H
#define SB_TRANSFORM2D_H

#include "sb_matrix2.h"

namespace Sandbox {
	
    struct Transform2d {
        Matrix2f    m;
        Vector2f    v;
        Transform2d() : m(Matrix2f::identity()),v(0.0f,0.0f) {}
        Transform2d(const Transform2d& other) : m(other.m),v(other.v){
        }
        Transform2d& operator = (const Transform2d& other) {
            m = other.m;
            v = other.v;
            return *this;
        }
        void reset() {
            m = Matrix2f::identity();
            v = Vector2f(0.0f,0.0f);
        }
        bool inverse() {
            float det = m.get_determinant();
            if (det != 0.0f) {
                m.inverse(det);
                v = - (m * v);
                return true;
            }
            return false;
        }
        Transform2d inverted() const {
            Transform2d tr(*this);
            tr.inverse();
            return tr;
        }
        Transform2d& translate(const Vector2f& pos) {
            v+=m*pos;
            return *this;
        }
        Transform2d& translate(float x,float y) {
            return translate(Vector2f(x,y));
        }
		Transform2d translated(float x,float y) const {
			Transform2d res(*this);
			res.translate(x,y);
			return res;
		}
		Transform2d translated(const Vector2f& pos) const {
			return Transform2d(*this).translate(pos);
		}
        /// rotate, radians
        Transform2d& rotate(float a) {
            float s,c;
            sincosf(a,s,c);
            m.rotate(s,c);
            return *this;
        }
        Transform2d& rotate_scale(float a,float sx, float sy) {
            float s,c;
            sincosf(a,s,c);
            m.rotate_scale(s,c,sx,sy);
            return *this;
        }
        Transform2d& rotate(const Vector2f& dir) {
            float c = dir.x;
            float s = -dir.y;
            m.rotate(s,c);
            return *this;
        }
        // screw, radians
        Transform2d& screw_x(float a) {
            float s,c;
            sincosf(a,s,c);
            m*=Matrix2f(c,0,-s,1);
            return *this;
        }
		Transform2d rotated(float a) const {
			Transform2d res(*this);
			return Transform2d(*this).rotate(a);
		}
        Transform2d rotated(const Vector2f& dir) const {
            Transform2d res(*this);
            return Transform2d(*this).rotate(dir);
        }
        Transform2d& scale(float s) {
            m.scale(s,s);
            return *this;
        }
        Transform2d& scale(float sx,float sy) {
            m.scale(sx,sy);
            return *this;
        }
        Transform2d& scale(const Vector2f& s) {
            m.scale(s.x,s.y);
            return *this;
        }
        Transform2d scaled(float sx,float sy) const {
            Transform2d tr = *this;
            tr.scale(sx,sy);
            return tr;
        }
        
        Vector2f transform(const Vector2f& vert) const {
            return v + (m * vert);
        }
		inline void transform(float x,float y,float& ox,float& oy) const {
			ox = v.x+m.comp.a*x + m.comp.c*y;
			oy = v.y+m.comp.b*x + m.comp.d*y;
		}
        inline Vector2f transform(float x,float y) const {
            return Vector2f(    v.x+m.comp.a*x + m.comp.c*y,
                                v.y+m.comp.b*x + m.comp.d*y);
        }
        Transform2d operator * (const Transform2d& tr) const {
            Transform2d res = *this;
            res *= tr;
            return res;
        }
        Transform2d& operator *= (const Transform2d& tr) {
            translate(tr.v);
            m *= tr.m;
            return *this;
        }
    };
	
    
    
    
}

#endif // SB_TRANSFORM2D_H
