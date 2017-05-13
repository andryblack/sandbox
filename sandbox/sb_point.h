/*
 *  sb_point.h
  *
 */

#ifndef SB_POINT_H
#define SB_POINT_H

#include "sb_vector2.h"
#include <sbstd/sb_algorithm.h>
#include "luabind/sb_luabind_stack.h"

namespace Sandbox {
	
	template <class T>
	struct Point {
		T x;
		T y;
		Point() {}
		Point(T _x,T _y) : x(_x),y(_y) {}
		bool operator == (const Point& r) const {
			return (x==r.x) && (y==r.y);
		}
        bool operator < (const Point& r) const {
            if (x == r.x) return y < r.y;
            return x < r.x;
        }
        Point operator + (const Point& v) const {
            return Point(x+v.x,y+v.y);
        }
        
        Point operator - (const Point& v) const {
            return Point(x-v.x,y-v.y);
        }
        
        Point& operator += (const Point& v) {
            x += v.x;
            y += v.y;
            return *this;
        }
        
        Point& operator -= (const Point& v) {
            x += v.x;
            y += v.y;
            return *this;
        }

        operator Vector2f () const {
            return Vector2f(float(x),float(y));
        }
	};
	
	typedef Point<float> Pointf;
	typedef Point<int> Pointi;
	
    namespace luabind {
        template <class T>
        struct stack<Point<T> > {
            typedef stack_help<Point<T>, false > help;
            static void push( lua_State* L, const Point<T>& val ) {
                help::push(L,val);
            }
            static Point<T> get( lua_State* L, int idx ) {
                if (lua_istable(L, idx)) {
                    Point<T> res;
                    lua_rawgeti(L, idx, 1);
                    res.x = lua_tonumber(L, -1);
                    lua_pop(L, 1);
                    lua_rawgeti(L, idx, 2);
                    res.y = lua_tonumber(L, -1);
                    lua_pop(L, 1);
                    return  res;
                }
                return help::get(L,idx);
            }
        };
        template <class T>
        struct stack<const Point<T>&> : stack<Point<T> > {};
        
    }
}

#endif /*SB_POINT_H*/
