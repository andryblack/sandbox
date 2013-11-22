//
//  sb_rocket_file.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/11/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_rocket_file.h"
#include "sb_resources.h"
#include <ghl_data_stream.h>

namespace Sandbox {
    
    inline static Rocket::Core::FileHandle cast_file(GHL::DataStream* ds) {
        return reinterpret_cast<Rocket::Core::FileHandle>(ds);
    }
    inline static GHL::DataStream* cast_file(Rocket::Core::FileHandle handle) {
        return reinterpret_cast<GHL::DataStream*>(handle);
    }
    
    RocketFileInterface::RocketFileInterface(Resources* res) : m_resources(res) {
        
    }
    
    
    Rocket::Core::FileHandle RocketFileInterface::Open(const Rocket::Core::String& path) {
        GHL::DataStream* ds = m_resources->OpenFile(path.CString());
        return cast_file(ds);
    }
   
    void RocketFileInterface::Close(Rocket::Core::FileHandle file) {
        cast_file(file)->Release();
    }
    
    size_t RocketFileInterface::Read(void* buffer, size_t size, Rocket::Core::FileHandle file) {
        GHL::UInt32 readed = cast_file(file)->Read(reinterpret_cast<GHL::Byte*>(buffer), GHL::UInt32(size));
        return readed;
    }
    
    bool RocketFileInterface::Seek(Rocket::Core::FileHandle file, long offset, int origin) {
        GHL::FileSeekType st = GHL::F_SEEK_BEGIN;
        if (origin == SEEK_CUR)
            st = GHL::F_SEEK_CURRENT;
        else if (origin == SEEK_END)
            st = GHL::F_SEEK_END;
        return cast_file(file)->Seek(GHL::Int32(offset), st);
    }
    
    size_t RocketFileInterface::Tell(Rocket::Core::FileHandle file) {
        return cast_file(file)->Tell();
    }
    
    
}

