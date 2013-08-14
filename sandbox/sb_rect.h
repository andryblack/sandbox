/*
 *  sb_rect.h
 *  SR
 *
 *  Created by Андрей Куницын on 29.05.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_RECT_H
#define SB_RECT_H

#include "sb_vector2.h"

namespace Sandbox {
	
	template <class T>
	struct Rect {
		T x;
		T y;
		T w;
		T h;
		Rect() {}
		Rect(T _x,T _y,T _w,T _h) : x(_x),y(_y),w(_w),h(_h) {}
		bool operator == (const Rect& r) const {
			return (x==r.x) && (y==r.y) && (w==r.w) && (h==r.h);
		}
        
        bool PointInside( const Vector2f& pos ) const {
            return (pos.x>=x) && (pos.y>=y) &&
            (pos.x<(x+w)) && (pos.y<(y+h));
        }
        
        Vector2f GetTopLeft() const {
            return Vector2f( x,y );
        }
        Vector2f GetSize() const {
            return Vector2f( w, h );
        }
        
        void SetPos(const Vector2f& f) {
            x = f.x;
            y = f.y;
        }
        void SetSize(const Vector2f& f) {
            w = f.x;
            h = f.y;
        }
	};
	
	typedef Rect<float> Rectf;
	typedef Rect<int> Recti;
	
}

#endif /*SB_RECT_H*/
