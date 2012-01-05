//
//  sb_particles_bind.cpp
//  pairs
//
//  Created by Андрей Куницын on 1/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "sb_particles.h"
#include "sb_lua.h"
#include "sb_bind.h"

namespace Sandbox {
    
    void ParticlesSystem::Bind( Lua* lua ) {
        SB_BIND_BEGIN_BIND
        {
            SB_BIND_BEGIN_EXTERN_CLASS(Sandbox::ParticleProcessorBlock)
            SB_BIND_END_CLASS
            SB_BIND(lua);
        }
        {
            SB_BIND_BEGIN_SHARED_CLASS(Sandbox::ParticlesController)
            SB_BIND_SHARED_CONSTRUCTOR(Sandbox::ParticlesController, ())
            SB_BIND_BEGIN_METHODS
            SB_BIND_METHOD(Sandbox::ParticlesController, AddImage, void(Sandbox::Image))
            SB_BIND_METHOD(Sandbox::ParticlesController, AddProcessor, void(const Sandbox::ParticleProcessorBlock,string_map<float>))
            SB_BIND_END_METHODS
            SB_BIND_BEGIN_PROPERTYS
            SB_BIND_PROPERTY_RW(Sandbox::ParticlesController, MaxAmount, GetMaxAmount, SetMaxAmount, size_t)
            SB_BIND_PROPERTY_RW_DEF(Sandbox::ParticlesController,EmissionSpeed,size_t)
            SB_BIND_PROPERTY_RW_DEF(Sandbox::ParticlesController,Loop,bool)
            SB_BIND_PROPERTY_RW_DEF(Sandbox::ParticlesController,CicleTime,float)
            SB_BIND_PROPERTY_RW_DEF(Sandbox::ParticlesController,ParticleTTL,float)
            SB_BIND_END_PROPERTYS
            SB_BIND_END_CLASS
            SB_BIND(lua)
        }
        {
            SB_BIND_BEGIN_SHARED_SUBCLASS(Sandbox::ParticlesSystem, Sandbox::Object)
            SB_BIND_SHARED_CONSTRUCTOR_(Sandbox::ParticlesSystem, (Sandbox::ParticlesController),(const Sandbox::ParticlesControllerPtr&))
            SB_BIND_BEGIN_METHODS
            SB_BIND_END_METHODS
            SB_BIND_BEGIN_PROPERTYS
            SB_BIND_PROPERTY_RW(Sandbox::ParticlesSystem, Position, GetPosition, SetPosition, Sandbox::Vector2f)
            SB_BIND_END_PROPERTYS
            SB_BIND_END_CLASS
            SB_BIND(lua)
        }
        SB_BIND_END_BIND
        BindBuiltins(lua);
    }
    void ParticlesSystem::BindProcessor( Lua* lua, const char* name,const ParticleProcessorBlock* block) {
        lua->SetValue(block, (std::string("Sandbox.ParticleProcessorBlock.")+name).c_str(), "Sandbox::ParticleProcessorBlock");
    }
    
}
