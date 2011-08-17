#ifndef SB_MATH_H
#define SB_MATH_H

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Sandbox {
	
	
	using std::pow;
	using std::sin;
	using std::cos;
	using std::asin;
	using std::acos;
	
	using std::sqrt;
	using std::floor;
	
	template <typename T> 
	inline T interpolate(T begin,T end,float k) {
		return end * k + begin * (1.0f-k);
	}
	
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
}

#endif /*SB_MATH_H*/