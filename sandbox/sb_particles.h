//
//  sb_particles.h
//  pairs
//
//  Created by Андрей Куницын on 1/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef pairs_sb_particles_h
#define pairs_sb_particles_h

#include "sb_object.h"
#include "sb_image.h"
#include "sb_particle.h"
#include "sb_event.h"

#include <map>

namespace Sandbox {
    
    class Lua;
    
    struct ParticlesInstance {
        std::vector<Particle>   particles;
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
        const char* names[];
    };
    typedef const ParticleProcessorBlock* ParticleProcessorBlockPtr;
    
    class ParticlesController {
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
        
        typedef std::map<std::string,float> ArgsMap;
        void AddProcessor( const ParticleProcessorBlock* block, 
                          const ArgsMap& args = ArgsMap());
        
        void GetImages( std::vector<const Image*>& images );
        
        void Update( ParticlesInstance& instance, float dt);
        void Start( ParticlesInstance& instance );
        
        void Emmit( ParticlesInstance& instance, size_t indx, float time ) const;
    private:
        size_t  m_max_amount;
        float   m_emission_speed;
        std::vector<ImagePtr>   m_images;
        bool    m_loop;
        float   m_cicle_time;
        float   m_args[32];
        std::vector<ParticleProcessorBlockPtr> m_processors;
        float   m_particle_ttl;
    };
    typedef shared_ptr<ParticlesController> ParticlesControllerPtr;
    
    class ParticlesSystem : public Object {
    public:
        explicit ParticlesSystem(const ParticlesControllerPtr& controller);
        void Draw( Graphics& g ) const;
        void Update( float dt );
        
        static void Bind( Lua* lua );
        static void BindProcessor( Lua* lua, const char* name,const ParticleProcessorBlock* block);
        
        void SetPosition( const Sandbox::Vector2f& pos ) { m_instance.pos = pos; }
        const Vector2f& GetPosition() const { return m_instance.pos; }
        
        void SetCompleteEvent( const EventPtr& evnt ) { m_instance.complete_event = evnt; }
    private:
        ParticlesControllerPtr  m_controller;
        ParticlesInstance   m_instance;
        static void BindBuiltins( Lua* lua );
    };
    typedef shared_ptr<ParticlesSystem> ParticlesSystemPtr;
}


#endif
