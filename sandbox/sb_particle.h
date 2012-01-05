//
//  sb_particle.h
//  pairs
//
//  Created by Андрей Куницын on 1/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef pairs_sb_particle_h
#define pairs_sb_particle_h

#include "sb_vector2.h"
#include "sb_color.h"

namespace Sandbox {
    
    struct Particle {
        /// visible params
        Vector2f    pos;
        float       scale;
        float       angle;
        Color       color;
        int         image;
        /// processor
        Vector2f    speed;
        float   ttl;
        float   ttlSpeed;
        float   vars[16];
    };
    
}

#endif
