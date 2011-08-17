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

	float randf() {
		return float(rand())/float(RAND_MAX);
	}
	float randf(float  from,float to) {
		return from + (to-from)*randf();
	}
}