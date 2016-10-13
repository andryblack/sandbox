/*
 *  sb_math.cpp
 *  SR
 *
 *  Created by Андрей Куницын on 06.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#include "sb_math.h"

#include "sb_vector2.h"
#include "sb_vector3.h"
#include "sb_vector4.h"
#include "sb_matrix2.h"
#include "sb_matrix3.h"
#include "sb_matrix4.h"

#include <cstdlib>

namespace Sandbox {

    size_t rand() {
        return ::rand();
    }
	float randf() {
		return float(rand())/float(RAND_MAX);
	}
	float randf(float  from,float to) {
		return from + (to-from)*randf();
	}
    
    bool intersect(const Vector2f& a1, const Vector2f& a2, const Vector2f& b1, const Vector2f& b2, Vector2f& res) {
        float x1 = a1.x, y1 = a1.y;
        float x2 = a2.x, y2 = a2.y;
        float x3 = b1.x, y3 = b1.y;
        float x4 = b2.x, y4 = b2.y;
        
        float d = ((y4-y3)*(x2-x1)-(x4-x3)*(y2-y1));
        
        if (d==0.0f)
            return false;
        
        float Ua=((x4-x3)*(y1-y3)-(y4-y3)*(x1-x3))/d;
        float Ub=((x2-x1)*(y1-y3)-(y2-y1)*(x1-x3))/d;
        
        if (Ua<0.0f || Ua>1.0f || Ub<0.0f || Ub>1.0f)
            return false;
        
        res.x=x1+Ua*(x2-x1);
        res.y=y1+Ua*(y2-y1);
  
        return true;
    }
    
    Vector2f intersect_lines(const Vector2f& a1, const Vector2f& a2, const Vector2f& b1, const Vector2f& b2) {
        float x1 = a1.x, y1 = a1.y;
        float x2 = a2.x, y2 = a2.y;
        float x3 = b1.x, y3 = b1.y;
        float x4 = b2.x, y4 = b2.y;
        
        float d = ((y4-y3)*(x2-x1)-(x4-x3)*(y2-y1));
        
        if (d==0.0f)
            return (a1+a2+b1+b2) / 4;
        
        float Ua=((x4-x3)*(y1-y3)-(y4-y3)*(x1-x3))/d;

        return Vector2f(x1+Ua*(x2-x1),y1+Ua*(y2-y1));
    }
}
