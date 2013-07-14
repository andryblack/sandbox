//
//  sb_particles_blocks.cpp
//  pairs
//
//  Created by Андрей Куницын on 1/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "sb_particles.h"
#include <cstdlib>

namespace Sandbox {
    
    static bool alpha_fade_out_processor( Particle& p, float* , const float* args, float , float  ) {
        const float from = args[0];
        if (p.ttl > from)
            p.color.a = 1.0f - (p.ttl-from)/(1.0f-from);
        return false;
    }
    
    
    
#define BLOCK_BEGIN(name) \
    static const char* name##_names[] = 
#define BLOCK_END(name,vars,g,p) ; \
    static const ParticleProcessorBlock name = { \
        sizeof(name##_names)/sizeof(name##_names[0]),vars,g,p,name##_names};
    
    BLOCK_BEGIN(alfa_fade_processor)
    {"from"}
    BLOCK_END(alfa_fade_processor,0, 0, &alpha_fade_out_processor)
    
    
    static void random_dir_generator( Particle& p, float* , const float* args, float  ) {
        Vector2f dir(0,1);
        dir.rotate(randf(-fPI, fPI));
        p.speed+=dir * randf(args[0],args[1]);
    }
    BLOCK_BEGIN(random_dir_processor)
    {"speed_min","speed_max"}
    BLOCK_END(random_dir_processor,0,&random_dir_generator,0);    
   
    
    static bool speed_accept_processor(Particle& p, float* , const float* , float dt, float ) {
        p.pos+=p.speed*dt;
        return false;
    }
    BLOCK_BEGIN(apply_speed_processor)
    {0}
    BLOCK_END(apply_speed_processor,0,0,&speed_accept_processor)
    
    static void random_pos_generator( Particle& p, float* , const float* args, float  ) {
        p.pos.x+=randf(args[0],args[1]);
        p.pos.y+=randf(args[2],args[3]);
    }
    BLOCK_BEGIN(random_pos_processor)
    {"delta_x_min","delta_x_max","delta_y_min","delta_y_max"}
    BLOCK_END(random_pos_processor, 0, &random_pos_generator, 0)
    
    static void ranndom_scale_generator( Particle& p, float* , const float* args, float  ) {
        p.scale = randf(args[0],args[1]);
    }
    BLOCK_BEGIN(random_scale_processor)
    {"min","max"}
    BLOCK_END(random_scale_processor, 0, &ranndom_scale_generator, 0)
     
    static void ranndom_image_generator( Particle& p, float* , const float* args, float  ) {
        p.image = rand()%static_cast<int>(args[0]);
    }
    BLOCK_BEGIN(random_image_processor)
    {"amount"}
    BLOCK_END(random_image_processor, 0, &ranndom_image_generator, 0)
    

    
    static bool gravity_accept_processor(Particle& p, float* , const float* args, float dt, float ) {
        p.speed.x+=args[0]*dt;
        p.speed.y+=args[1]*dt;
        return false;
    }
    BLOCK_BEGIN(apply_gravity_processor)
    {"x","y"}
    BLOCK_END(apply_gravity_processor, 0, 0, &gravity_accept_processor)
    
    
    
    static void scale_accept_generator( Particle& , float* vars, const float* args, float  ) {
        vars[0] = randf(args[0],args[1]);
    }
    static bool scale_accept_processor(Particle& p, float* vars, const float* , float dt, float ) {
        p.scale+=vars[0]*dt;
        return false;
    }
    BLOCK_BEGIN(scale_speed_processor)
    {"min","max"}
    BLOCK_END(scale_speed_processor, 1, &scale_accept_generator, &scale_accept_processor)
   
    
    
    static void circle_generator( Particle& p, float* , const float* args, float cicle ) {
        Vector2f dir = Vector2f(0,1).rotate(cicle*fPI*2*args[0]);
        p.pos+=dir*randf(args[1],args[2]);
        p.speed+=dir*randf(args[3],args[4]);
    }
   
    BLOCK_BEGIN(circle_processor)
    {"cicles","r_min","r_max","speed_min","speed_max"}
    BLOCK_END(circle_processor, 0, &circle_generator, 0)
        
    void ParticlesSystem::BindBuiltins( LuaVM* lua ) {
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
