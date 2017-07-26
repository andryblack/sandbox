#ifndef SB_QUATERNION_H_INCLUDED
#define SB_QUATERNION_H_INCLUDED


#include "sb_vector4.h"

namespace Sandbox {
    
    
    
    /// Quaternion
    struct Quaternion
    {
        
        float x;
        float y;
        float z;
        float w;
        
        Quaternion() : x(0), y(0), z(0), w(0) { }
        Quaternion( float x, float y , float z , float w ) : x(x), y(y), z(z), w(w) { }
        template <class T>
        explicit Quaternion( const T& v) : x(v.x),y(v.y),z(v.z),w(v.w) {}
        
        
        
    public:
        
        
    public:
        
        Quaternion &operator = (const Quaternion& o) { x = o.x; y = o.y; z = o.z; w = o.w; return *this; }
        bool operator == (const Quaternion& o) const {return ((x == o.x) && (y == o.y) && (z == o.z) && (w == o.w));}
        bool operator != (const Quaternion& o) const {return ((x != o.x) || (y != o.y) || (z != o.z) || (w != o.w));}
        
    public:
        Quaternion operator + (const Quaternion& o) const {
            return Quaternion(x+o.x,y+o.y,z+o.z,w+o.w);
        }
        Quaternion& operator += (const Quaternion& o) {
            x+=o.x;y+=o.y;z+=o.z;w+=o.w; return *this;
        }
        Quaternion operator - (const Quaternion& o) const {
            return Quaternion(x-o.x,y-o.y,z-o.z,w-o.w);
        }
        Quaternion& operator -= (const Quaternion& o) {
            x-=o.x;y-=o.y;z-=o.z;w-=o.w; return *this;
        }
        Quaternion operator * (const Quaternion& q) const {
            return Quaternion(
                              w*q.x + x*q.w + y*q.z - z*q.y,
                              w*q.y + y*q.w + z*q.x - x*q.z,
                              w*q.z + z*q.w + x*q.y - y*q.x,
                              w*q.w - x*q.x - y*q.y - z*q.z);
        }
        Quaternion& operator *= (const Quaternion& q) {
            *this = (*this) * q ; return *this;
        }
        
        Quaternion operator * (float s) const {
            return Quaternion(x*s,y*s,z*s,w*s);
        }
        Quaternion& operator *= (float s) {
            x*=s;y*=s;z*=s;w*=s; return *this;
        }
        
        float magnitude() const {
            return ::sqrtf(x*x+y*y+z*z+w*w);
        }
        
        Quaternion normalized() const {
            float im = 1.0f / magnitude();
            return Quaternion(x*im,y*im,z*im,w*im);
        }
    };
    
    namespace luabind {
        template <>
        struct stack<Quaternion> {
            typedef stack_help<Quaternion, false > help;
            static void push( lua_State* L, const Quaternion& val ) {
                help::push(L,val);
            }
            static Quaternion get( lua_State* L, int idx ) {
                if (lua_istable(L, idx)) {
                    Quaternion res;
                    lua_rawgeti(L, idx, 1);
                    res.x = float(lua_tonumber(L, -1));
                    lua_pop(L, 1);
                    lua_rawgeti(L, idx, 2);
                    res.y = float(lua_tonumber(L, -1));
                    lua_pop(L, 1);
                    lua_rawgeti(L, idx, 3);
                    res.z = float(lua_tonumber(L, -1));
                    lua_pop(L, 1);
                    lua_rawgeti(L, idx, 4);
                    res.w = float(lua_tonumber(L, -1));
                    lua_pop(L, 1);
                    return  res;
                }
                return help::get(L,idx);
            }
        };
        template <>
        struct stack<const Quaternion&> : stack<Quaternion> {};
        
    }

}

#endif /*SB_QUATERNION_H_INCLUDED*/
