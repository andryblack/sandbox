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

namespace Sandbox {
	
	template <class T>
	struct Rect {
		T x;
		T y;
		T w;
		T h;
		Rect() {}
		Rect(T _x,T _y,T _w,T _h) : x(_x),y(_y),w(_w),h(_h) {}
	};
	
	typedef Rect<float> Rectf;
	typedef Rect<int> Recti;
	
}

#endif /*SB_RECT_H*/
