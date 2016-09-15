//
//  sb_particles.h
//  pairs
//
//  Created by Андрей Куницын on 1/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef pairs_sb_particles_h
#define pairs_sb_particles_h

#include "sb_scene_object.h"
#include "sb_image.h"
#include "sb_particle.h"
#include "sb_event.h"
#include <sbstd/sb_vector.h>
#include <sbstd/sb_map.h>
#include <sbstd/sb_string.h>


namespace Sandbox {
    
    class LuaVM;
    
    struct ParticlesInstance {
        sb::vector<Particle>   particles;
        float   time;
        float   cicle_time;
        bool    started;
        bool    finished;
        Vector2f    pos;
        EventPtr    complete_event;
    };
  
    typedef void (*GeneratorFunc)( Particle& p, float* vars, const float* args, float cicle );
    typedef bool (*ProcessorFunc)( Particle& p, float* vars, const float* args, float dt, float cicle );
    
    struct ParticleProcessorBlock {
        const size_t  num_args;
        const size_t  num_vars;
        GeneratorFunc   generator;
        ProcessorFunc   processor;
        const char** names;
    };
    typedef const ParticleProcessorBlock* ParticleProcessorBlockPtr;
    
    class ParticlesController : public sb::ref_countered_base {
    public:
        ParticlesController();
        
        size_t  GetMaxAmount() const { return m_max_amount;}
        void    SetMaxAmount(size_t count) { m_max_amount = count; }
        
        float   GetEmissionSpeed() const { return m_emission_speed;}
        void    SetEmissionSpeed(float speed) { m_emission_speed = speed;}
  
        bool    GetLoop() const { return m_loop; }
        void    SetLoop(bool loop) { m_loop = loop; }
        
        float   GetCicleTime() const { return m_cicle_time;}
        void    SetCicleTime(float t) { m_cicle_time = t;}
        
        float   GetParticleTTL() const { return m_particle_ttl;}
        void    SetParticleTTL(float ttl) { m_particle_ttl=ttl;}
        
        void AddImage( const ImagePtr& img );
        
        typedef sb::map<sb::string,float> ArgsMap;
        void AddProcessor( const ParticleProcessorBlock* block, 
                          const ArgsMap& args = ArgsMap());
        
        void GetImages( std::vector<const Image*>& images );
        const std::vector<ImagePtr>&  GetImages() const { return m_images; }
        
        virtual void Update( ParticlesInstance& instance, float dt);
        void Start( ParticlesInstance& instance );
        
        virtual void Emmit( ParticlesInstance& instance, 
                   size_t indx, 
                   float time ) const;
    private:
        size_t  m_max_amount;
        float   m_emission_speed;
        sb::vector<ImagePtr>   m_images;
        bool    m_loop;
        float   m_cicle_time;
        float   m_args[32];
        sb::vector<ParticleProcessorBlockPtr> m_processors;
        float   m_particle_ttl;
    };
    typedef sb::intrusive_ptr<ParticlesController> ParticlesControllerPtr;
    
    class ParticlesSystem : public SceneObjectWithPosition {
        SB_META_OBJECT
    public:
        explicit ParticlesSystem(const ParticlesControllerPtr& controller);
        void Draw( Graphics& g ) const;
        void Update( float dt );
        
        static void Bind( LuaVM* lua );
        static void BindProcessor( LuaVM* lua, const char* name,const ParticleProcessorBlock* block);
        
        void SetCompleteEvent( const EventPtr& evnt ) { m_instance.complete_event = evnt; }
        const EventPtr& GetCompleteEvent() const { return m_instance.complete_event; }
        
        void Stop();
    private:
        ParticlesControllerPtr  m_controller;
        ParticlesInstance   m_instance;
        static void BindBuiltins( LuaVM* lua );
    };
    typedef sb::intrusive_ptr<ParticlesSystem> ParticlesSystemPtr;
}


#endif
