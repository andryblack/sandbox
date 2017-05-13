//
//  sb_particles.cpp
//  pairs
//
//  Created by Андрей Куницын on 1/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "sb_particles.h"
#include "sb_graphics.h"
#include "sb_log.h"

SB_META_DECLARE_OBJECT(Sandbox::ParticlesSystem, Sandbox::SceneObjectWithPosition)

namespace Sandbox {
    
    static const char* MODULE = "Sandbox:Particles";
    
    
    
    ParticlesController::ParticlesController() 
    : m_max_amount(128)
    , m_emission_speed(64.0f)
    , m_loop(true)
    , m_cicle_time( 5.0f )
    , m_particle_ttl(1.0f)
    {
        
    }
    
    
    void ParticlesController::AddImage( const ImagePtr& img ) {
        m_images.push_back( img );
    }
    void ParticlesController::AddProcessor( const ParticleProcessorBlock* block, const std::map<std::string,float>& argsList) {
        float* args = m_args;
        for (size_t i=0;i<m_processors.size();i++) {
            args+=m_processors[i]->num_args;
        }
        for (size_t i=0;i<block->num_args;++i) {
            const char* name = block->names[i];
            std::map<std::string,float>::const_iterator it = argsList.find(name);
            if (it!=argsList.end())
                *args = it->second;
            else {
                LogWarning(MODULE) << "Not found argument : " << name;
                *args = 0.0f;
            }
            ++args;
        }
        m_processors.push_back( block );
    }
    
    void ParticlesController::GetImages( std::vector<const Image*>& images ) {
        images.reserve(m_images.size());
        for (size_t i=0;i<m_images.size();++i) {
            images.push_back(m_images[i].get());
        }
    }
    
    void ParticlesController::Emmit( ParticlesInstance& instance, size_t indx,float time ) const {
        Particle& p = instance.particles[indx];
        p.pos = instance.pos;
        p.scale = 1.0f;
        p.angle = 0.0f;
        p.image = 0;
        p.ttl = 0.0f;
        p.ttlSpeed = 1.0f/m_particle_ttl;
        
        const float* args = m_args;
        float* vars = p.vars;
        for (size_t i=0;i<m_processors.size();++i) {
            if (m_processors[i]->generator) m_processors[i]->generator(p,vars,args,time);
            args+=m_processors[i]->num_args;
            vars+=m_processors[i]->num_vars;
        }
    }
    void ParticlesController::Start( ParticlesInstance& instance ) {
        instance.started = true;
        instance.cicle_time = GetCicleTime();
    }
    void ParticlesController::Update( ParticlesInstance& instance, float dt) {
        if (instance.finished) {
            if (!instance.particles.empty()) {
                instance.particles.clear();
            }
            return;
        }
        instance.time+=dt;
        float startTime = 1.0f - (instance.cicle_time) / m_cicle_time;
        instance.cicle_time-=dt;
        int  emmit_amount = static_cast<int>(instance.time * m_emission_speed);
        if (!instance.started) {
            emmit_amount = 0;
        } else {
            if (instance.cicle_time<=0.0f) {
                if (!m_loop) {
                    instance.started = false;
                    instance.time += instance.cicle_time;
                    emmit_amount = static_cast<int>(instance.time * m_emission_speed);
                } else {
                    if (instance.started) Start( instance );
                }
            }
        }
        
        std::vector<Particle>::iterator it = instance.particles.begin();
        while (it!=instance.particles.end()) {
            const float* args = m_args;
            float* vars = it->vars;
            std::vector<Particle>::iterator next = it+1;
            for (size_t i=0;i<m_processors.size();++i) {
                it->ttl+=it->ttlSpeed * dt;
                if ((it->ttl>1.0f) || 
                    ( m_processors[i]->processor && 
                     m_processors[i]->processor(*it,vars,args,dt,startTime))) {
                    next = instance.particles.erase(it);
                    break;
                }
                args+=m_processors[i]->num_args;
                vars+=m_processors[i]->num_vars;
            }
            it = next;
        }
        if (emmit_amount > int(m_max_amount))
            emmit_amount = m_max_amount;
        //emmit_amount = std::min(emmit_amount,int(m_max_amount)-int(instance.particles.size()));
        int free_particles = (int(m_max_amount)-int(instance.particles.size()));
        //LogVerbose() << "emit: " << emmit_amount << " free: " << free_particles << " dt:" << dt << " circle: " << instance.cicle_time;
        if (emmit_amount>free_particles) {
            /// force dead
            int force_dead = emmit_amount-free_particles;
            instance.particles.erase(instance.particles.begin(),instance.particles.begin()+force_dead);
        }
        if (emmit_amount>0) {
            
            const size_t base_index = instance.particles.size();
            instance.particles.resize( base_index+emmit_amount );
            float emissionStep = (instance.time/emmit_amount)/m_cicle_time;
            for (int i=0;i<emmit_amount;i++) {
                //LogVerbose() << "emit at " << startTime;
                Emmit( instance, base_index+i, startTime );
                startTime+=emissionStep;
                if (startTime>1.0f)
                    startTime-=1.0f;
            }
            instance.time-=emmit_amount * (1.0f/m_emission_speed);
        }
        if (!instance.started) {
            if ( instance.particles.empty() ) {
                instance.finished = true;
                if (instance.complete_signal)
                    instance.complete_signal->Emmit();
            }
        }
    }
    
    
    ParticlesSystem::ParticlesSystem(const ParticlesControllerPtr& controller) : m_controller(controller) {
        if (m_controller) {
            m_instance.particles.reserve(controller->GetMaxAmount());
            m_instance.cicle_time = m_controller->GetCicleTime();
        }
        m_instance.time = 0.0f;
        m_instance.started = true;
        m_instance.finished = false;
    }
    
    void ParticlesSystem::Draw(Sandbox::Graphics &g) const {
        if (m_instance.particles.empty()) return;
        if (!m_controller) return;
        g.DrawParticles(m_instance.particles, m_controller->GetImages());
    }
    
    void ParticlesSystem::Update( float dt ) {
        if (m_controller && !m_instance.finished) {
            m_instance.pos = GetPos();
            m_controller->Update(m_instance, dt);
        }
    }
    
    void ParticlesSystem::Stop() {
        m_instance.started = false;
    }
    
        
}
