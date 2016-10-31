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

#include "sb_point.h"
#include "sb_size.h"
#include <sbstd/sb_algorithm.h>
#include "luabind/sb_luabind_stack.h"

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
        
        Point<T> GetTopLeft() const {
            return Point<T>( x,y );
        }
        Point<T> GetBottomRight() const {
            return Point<T>( x + w, y + h);
        }
        Size<T> GetSize() const {
            return Size<T>( w, h );
        }
        
        void SetPos(const Point<T>& f) {
            x = f.x;
            y = f.y;
        }
        void SetSize(const Size<T>& f) {
            w = f.w;
            h = f.h;
        }
        T GetLeft() const { return x; }
        T GetRight() const { return x + w; }
        T GetTop() const { return y; }
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
        Rect operator + (const Point<T>& v) const {
            return Rect(x+v.x,y+v.y,w,h);
        }
        Rect operator - (const Point<T>& v) const {
            return Rect(x-v.x,y-v.y,w,h);
        }
	};
	
	typedef Rect<float> Rectf;
	typedef Rect<int> Recti;
	
    namespace luabind {
        template <class T>
        struct stack<Rect<T> > {
            typedef stack_help<Rect<T>, false > help;
            static void push( lua_State* L, const Rect<T>& val ) {
                help::push(L,val);
            }
            static Rect<T> get( lua_State* L, int idx ) {
                if (lua_istable(L, idx)) {
                    Rect<T> res;
                    lua_rawgeti(L, idx, 1);
                    res.x = lua_tonumber(L, -1);
                    lua_pop(L, 1);
                    lua_rawgeti(L, idx, 2);
                    res.y = lua_tonumber(L, -1);
                    lua_pop(L, 1);
                    lua_rawgeti(L, idx, 3);
                    res.w = lua_tonumber(L, -1);
                    lua_pop(L, 1);
                    lua_rawgeti(L, idx, 4);
                    res.h = lua_tonumber(L, -1);
                    lua_pop(L, 1);
                    return  res;
                }
                return help::get(L,idx);
            }
        };
        template <class T>
        struct stack<const Rect<T>&> : stack<Rect<T> > {};
        
    }
}

#endif /*SB_RECT_H*/
