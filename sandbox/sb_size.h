/*
 *  sb_size.h
 */

#ifndef SB_SIZE_H
#define SB_SIZE_H

#include "sb_vector2.h"
#include <sbstd/sb_algorithm.h>
#include "luabind/sb_luabind_stack.h"

namespace Sandbox {
    
    template <class T>
    struct Size {
        T w;
        T h;
        Size() {}
        Size(T _x,T _y) : w(_x),h(_y) {}
        template <class U>
        explicit Size(const U& s) : w(s.width),h(s.height) {}
        bool operator == (const Size& r) const {
            return (w==r.w) && (h==r.h);
        }
        bool operator < (const Size& r) const {
            if (h == r.h) return w < r.w;
            return h < r.h;
        }
        Size operator + (const Size& v) const {
            return Size(w+v.w,h+v.h);
        }
        
        template <class U>
        Size operator * (U v) const {
            return Size(w * v, h * v);
        }
        
        Size operator - (const Size& v) const {
            return Size(w-v.w,h-v.h);
        }
        
        Size& operator += (const Size& v) {
            w += v.w;
            h += v.h;
            return *this;
        }
        
        Size& operator -= (const Size& v) {
            w += v.w;
            h += v.w;
            return *this;
        }
        
        operator Vector2f () const {
            return Vector2f(w,h);
        }
    };
    
    typedef Size<float> Sizef;
    typedef Size<int> Sizei;
    
    namespace luabind {
        template <class T>
        struct stack<Size<T> > {
            typedef stack_help<Size<T>, false > help;
            static void push( lua_State* L, const Size<T>& val ) {
                help::push(L,val);
            }
            static Size<T> get( lua_State* L, int idx ) {
                if (lua_istable(L, idx)) {
                    Size<T> res;
                    lua_rawgeti(L, idx, 1);
                    res.w = lua_tonumber(L, -1);
                    lua_pop(L, 1);
                    lua_rawgeti(L, idx, 2);
                    res.h = lua_tonumber(L, -1);
                    lua_pop(L, 1);
                    return  res;
                }
                return help::get(L,idx);
            }
        };
        template <class T>
        struct stack<const Size<T>&> : stack<Size<T> > {};
        
    }
}

#endif /*SB_POINT_H*/
