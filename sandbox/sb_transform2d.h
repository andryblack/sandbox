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
		void inverse() {
            m.inverse();
            v = - (m * v);
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
        Transform2d& rotate(float _a) {
            float a = _a;
            float c = ::cosf(a);
            float s = ::sinf(a);
            m*=Matrix2f(c,s,-s,c);
            return *this;
        }
        // screw, radians
        Transform2d& screw_x(float _a) {
            float a = _a;
            float c = ::cosf(a);
            float s = ::sinf(a);
            m*=Matrix2f(c,0,-s,1);
            return *this;
        }
		Transform2d rotated(float a) const {
			Transform2d res(*this);
			return Transform2d(*this).rotate(a);
		}
        Transform2d& scale(float s) {
            m*=Matrix2f(s,0,0,s);
            return *this;
        }
        Transform2d& scale(float sx,float sy) {
            m*=Matrix2f(sx,0,0,sy);
            return *this;
        }
        Transform2d& scale(const Vector2f& s) {
            m*=Matrix2f(s.x,0,0,s.y);
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
			ox = v.x+m.matrix[0*2+0]*x + m.matrix[1*2+0]*y;
			oy = v.y+m.matrix[0*2+1]*x + m.matrix[1*2+1]*y;
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
