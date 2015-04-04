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
#include <sbstd/sb_algorithm.h>

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
        T GetRight() const { return x + w; }
        T GetBottom() const { return y + h; }
        
        Rect GetIntersect(const Rect& r2) const {
            Rect res;
            res.x = sb::max(x,r2.x);
            res.y = sb::max(y,r2.y);
            res.w = sb::min(GetRight(),r2.GetRight());
            res.h = sb::min(GetBottom(),r2.GetBottom());
            res.w -= res.x;
            res.h -= res.y;
            return res;
        }
        Rect operator - (const Sandbox::Vector2f& v) const {
            return Rect(x-v.x,y-v.y,w,h);
        }
	};
	
	typedef Rect<float> Rectf;
	typedef Rect<int> Recti;
	
}

#endif /*SB_RECT_H*/
