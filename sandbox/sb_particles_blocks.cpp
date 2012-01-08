//
//  sb_particles_blocks.cpp
//  pairs
//
//  Created by Андрей Куницын on 1/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "sb_particles.h"

namespace Sandbox {
    
    static bool alpha_fade_out_processor( Particle& p, float* vars, const float* args, float dt, float cicle ) {
        const float from = args[0];
        if (p.ttl > from)
            p.color.a = 1.0f - (p.ttl-from)/(1.0f-from);
        return false;
    }
    
    
    
    static const ParticleProcessorBlock alfa_fade_processor = { 
        1,0,
        0,
        &alpha_fade_out_processor,
        {"from"}
    };
    
    static void random_dir_generator( Particle& p, float* vars, const float* args, float cicle ) {
        Vector2f dir(0,1);
        dir.rotate(randf(-M_PI, M_PI));
        p.speed+=dir * randf(args[0],args[1]);
    }
        
    static const ParticleProcessorBlock random_dir_processor = { 
        2,0,
        &random_dir_generator,
        0,
        {"speed_min","speed_max"}
    };
    
    static bool speed_accept_processor(Particle& p, float* vars, const float* args, float dt, float cicle) {
        p.pos+=p.speed*dt;
        return false;
    }
    static const ParticleProcessorBlock apply_speed_processor = { 
        0,0,
        0,
        &speed_accept_processor,
        {0}
    };

    
    static void random_pos_generator( Particle& p, float* vars, const float* args, float cicle ) {
        p.pos.x+=randf(args[0],args[1]);
        p.pos.y+=randf(args[2],args[3]);
    }
    static const ParticleProcessorBlock random_pos_processor = { 
        4,0,
        &random_pos_generator,
        0,
        {"delta_x_min","delta_x_max","delta_y_min","delta_y_max"}
    };
    
    static void ranndom_scale_generator( Particle& p, float* vars, const float* args, float cicle ) {
        p.scale = randf(args[0],args[1]);
    }
    static const ParticleProcessorBlock random_scale_processor = { 
        2,0,
        &ranndom_scale_generator,
        0,
        {"min","max"}
    };
    
    static void ranndom_image_generator( Particle& p, float* vars, const float* args, float cicle ) {
        p.image = rand()%static_cast<int>(args[0]);
    }
    static const ParticleProcessorBlock random_image_processor = { 
        1,0,
        &ranndom_image_generator,
        0,
        {"amount"}
    };

    
    static bool gravity_accept_processor(Particle& p, float* vars, const float* args, float dt, float cicle) {
        p.speed.x+=args[0]*dt;
        p.speed.y+=args[1]*dt;
        return false;
    }
    static const ParticleProcessorBlock apply_gravity_processor = { 
        2,0,
        0,
        &gravity_accept_processor,
        {"x","y"}
    };
    
    static void scale_accept_generator( Particle& p, float* vars, const float* args, float cicle ) {
        vars[0] = randf(args[0],args[1]);
    }
    static bool scale_accept_processor(Particle& p, float* vars, const float* args, float dt, float cicle) {
        p.scale+=vars[0]*dt;
        return false;
    }
    static const ParticleProcessorBlock scale_speed_processor = { 
        2,1,
        &scale_accept_generator,
        &scale_accept_processor,
        {"min","max"}
    };
    
    
    static void circle_generator( Particle& p, float* vars, const float* args, float cicle ) {
        Vector2f dir = Vector2f(0,1).rotate(cicle*M_PI*2*args[0]);
        p.pos+=dir*randf(args[1],args[2]);
        p.speed+=dir*randf(args[3],args[4]);
    }
   
    static const ParticleProcessorBlock circle_processor = { 
        5,0,
        &circle_generator,
        0,
        {"cicles","r_min","r_max","speed_min","speed_max"}
    };
    
    void ParticlesSystem::BindBuiltins( Lua* lua ) {
        BindProcessor(lua, "AlphaFade", &alfa_fade_processor);
        BindProcessor(lua, "RandomPos", &random_pos_processor);
        BindProcessor(lua, "RandomDir", &random_dir_processor);
        BindProcessor(lua, "ApplySpeed", &apply_speed_processor);
        BindProcessor(lua, "RandomScale", &random_scale_processor);
        BindProcessor(lua, "ScaleSpeed", &scale_speed_processor);
        BindProcessor(lua, "RandomImage", &random_image_processor);
        BindProcessor(lua, "ApplyGravity", &apply_gravity_processor);
        BindProcessor(lua, "Circle", &circle_processor);
    }
    


}