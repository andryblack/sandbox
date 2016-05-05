//
//  sb_memory_mgr.h
//  backgammon-osx
//
//  Created by Andrey Kunitsyn on 12/21/12.
//  Copyright (c) 2012 Andrey Kunitsyn. All rights reserved.
//

#ifndef __backgammon_osx__sb_memory_mgr__
#define __backgammon_osx__sb_memory_mgr__

#include "sb_notcopyable.h"
#include <sbstd/sb_assert.h>
#include <sbstd/sb_vector.h>
#include <ghl_types.h>
#include <cstring>

#ifdef SB_DEBUG
#ifndef SB_SILENT
#define SB_MEMORY_MGR_STAT
#endif
#endif

namespace Sandbox {

    class MemoryMgr : public NotCopyable {
    private:
        typedef void* tlsf_t;
        typedef void* pool_t;
        static const size_t INITIAL_SIZE = 2*1024*1024;
        unsigned char m_initial[INITIAL_SIZE];
        static const size_t POOL_SIZE = 4 * 1024 * 1024;
        struct pool_block_t {
            pool_t pool;
            unsigned char mem[POOL_SIZE];
        };
        tlsf_t  m_tlsf;
        sb::vector<pool_block_t*> m_pools;
#ifdef SB_MEMORY_MGR_STAT
        size_t m_total_allocated;
#endif
        void append_pool();
    public:
        MemoryMgr();
        ~MemoryMgr();
        GHL::Byte* alloc( size_t size );
        void free( GHL::Byte* ptr , size_t size );
        GHL::Byte* realloc( GHL::Byte* ptr, size_t size, size_t nsize );
        GHL::UInt32 allocated() const;
    };
    
}

#endif /* defined(__backgammon_osx__sb_memory_mgr__) */
