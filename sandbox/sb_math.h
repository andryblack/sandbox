#ifndef SB_MATH_H
#define SB_MATH_H

#include <cmath>
#include <cstring>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace Sandbox {
	
	
	using std::pow;
	using std::sin;
	using std::cos;
	using std::asin;
	using std::acos;
    using std::atan2;
    
	using std::sqrt;
	using std::floor;
	
    static const float fPI = 3.14159265358979323846f;
    
	template <typename T> 
	inline T interpolate(T begin,T end,float k) {
		return end * k + begin * (1.0f-k);
	}
    
    size_t rand();
	float randf();
	float randf(float  from,float to);
	
	inline unsigned int next_pot(unsigned int val) {
        val--;
        val = (val >> 1) | val;
        val = (val >> 2) | val;
        val = (val >> 4) | val;
        val = (val >> 8) | val;
        val = (val >> 16) | val;
        val++;
        return val;
    }
    inline unsigned int prev_pot(unsigned int val) {
        unsigned int next =  next_pot( val );
        return next == val ? val : next / 2;
    }
    
    struct Vector2f;
    Vector2f intersect_lines(const Vector2f& a1, const Vector2f& a2, const Vector2f& b1, const Vector2f& b2);
    bool intersect(const Vector2f& a1, const Vector2f& a2, const Vector2f& b1, const Vector2f& b2, Vector2f& res);
    
    static inline void sincosf(float a,float& s,float& c) {
        
#if defined( __EMSCRIPTEN__ )
        EM_ASM({HEAPF32[$1>>2]=Math_sin($0);HEAPF32[$2>>2]=Math_cos($0);},a,&s,&c);
#elif defined(__APPLE__)
        __sincosf(a,&s,&c);
#elif defined(__clang__)
        c = __builtin_cosf(a);
        s = __builtin_sinf(a);
#else
        c = ::cosf(a);
        s = ::sinf(a);
#endif
    }
}

#endif /*SB_MATH_H*/
