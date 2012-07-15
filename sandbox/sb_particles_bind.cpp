//
//  sb_particles_bind.cpp
//  pairs
//
//  Created by Андрей Куницын on 1/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "sb_particles.h"
#include "sb_lua.h"
#include "luabind/sb_luabind.h"

SB_META_DECLARE_KLASS(Sandbox::ParticleProcessorBlock, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::ParticleProcessorBlock)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ParticlesController, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::ParticlesController)
SB_META_CONSTRUCTOR(())
SB_META_METHOD(AddImage)
SB_META_METHOD(AddProcessor)
SB_META_PROPERTY_RW(MaxAmount, GetMaxAmount, SetMaxAmount)
SB_META_PROPERTY_RW_DEF(EmissionSpeed)
SB_META_PROPERTY_RW_DEF(Loop)
SB_META_PROPERTY_RW_DEF(CicleTime)
SB_META_PROPERTY_RW_DEF(ParticleTTL)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::ParticlesSystem)
SB_META_CONSTRUCTOR((const Sandbox::ParticlesControllerPtr&))
SB_META_METHOD(Stop)
SB_META_PROPERTY_RW_DEF(Position)
SB_META_PROPERTY_RW_DEF(CompleteEvent)
SB_META_END_KLASS_BIND()



namespace Sandbox {
    
    void ParticlesSystem::Bind( LuaVM* lua ) {
        luabind::ExternClass<ParticleProcessorBlock>(lua->GetVM());
        luabind::Class<ParticlesController>(lua->GetVM());
        luabind::Class<ParticlesSystem>(lua->GetVM());
        BindBuiltins(lua);
    }

    void ParticlesSystem::BindProcessor( LuaVM* lua, const char* name,const ParticleProcessorBlock* block) {
        luabind::SetValue(lua->GetVM(), (std::string("Sandbox.ParticleProcessorBlock.")+name).c_str(), block);
    }
    
    
}
