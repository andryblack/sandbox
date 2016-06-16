//
//  sb_log.cpp
//

#include "sb_log.h"

#include <sbstd/sb_platform.h>
#include <ghl_vfs.h>
#include "meta/sb_meta.h"




namespace Sandbox {
    
    
    
    static const char* curr_log = "curr_log.txt";
    static const char* prev_log = "prev_log.txt";
    static GHL::Byte rn[] = {'\n'};
    
    bool Logger::m_enable_platform_log =
#ifdef SB_DEBUG
    true
#else
    false
#endif
    ;
    
    static const char* level_descr[] = {
        "F:",
        "E:",
        "W:",
        "I:",
        "V:",
        "D:"
    };

    class GHLLogger : public GHL::Logger {
        GHL::WriteStream*   m_file;
    public:
        GHLLogger() : m_file(0) {}
        ~GHLLogger() {
            close();
        }
        virtual void GHL_CALL AddMessage( GHL::LogLevel level, const char* message ) {
            if (m_file) {
                m_file->Write(reinterpret_cast<const GHL::Byte*>(level_descr[level]), 2);
                m_file->Write(reinterpret_cast<const GHL::Byte*>(message), ::strlen(message));
                m_file->Write(rn, sizeof(rn));
            }
        }
        void close() {
            if (m_file) {
                m_file->Close();
                m_file->Release();
                m_file = 0;
            }
        }
        
        bool open(GHL::VFS* vfs,const char* file) {
            close();
            m_file = vfs->OpenFileWrite(file);
            return m_file!=0;
        }
        bool opened() const { return m_file!=0; }
    };
    
    static GHLLogger file_logger;
    
    void Logger::StartSession(GHL::VFS* vfs) {
#if !defined(GHL_PLATFORM_EMSCRIPTEN) && !defined(GHL_PLATFORM_FLASH)
        sb::string path = vfs->GetDir(GHL::DIR_TYPE_USER_PROFILE);
        if (!path.empty()) {
            if (path[path.length()-1]!='/') {
                path += "/";
            }
        }
        file_logger.close();
        sb::string dst_path = path + curr_log;
        vfs->DoRenameFile(dst_path.c_str(),(path + prev_log).c_str());
        if (file_logger.open(vfs,dst_path.c_str())) {
            GHL_Log(GHL::LOG_LEVEL_INFO, (sb::string("start writing ") + dst_path).c_str() );
            GHL_SetLogger(&file_logger);
        }
#endif
    }
    
    Logger::Logger( GHL::LogLevel level , const char* module) :  m_module(module),m_level( level ){
        m_stream << "[" << (m_module ? m_module : "Sandbox") << "] ";
    }
    
    Logger::~Logger() {
        if (file_logger.opened()) {
            file_logger.AddMessage(m_level, m_stream.str().c_str());
        }

        if (m_enable_platform_log)
        {
            GHL_Log(m_level, m_stream.str().c_str());
        }
    }
    
    void format_memory( char* buf, size_t size, size_t mem,const char* caption ) {
        if ( mem > 1024*1024 ) {
            sb::snprintf(buf, size, "%s%0.2fM", caption,float(mem)/(1024*1024));
        } else if ( mem > 1024 ) {
            sb::snprintf(buf, size, "%s%0.2fK", caption,float(mem)/(1024));
        }
        else {
            sb::snprintf(buf, size, "%s%0.2fb", caption,float(mem));
        }
    }
    
    sb::string format_memory( size_t mem ) {
        char buf[64];
        format_memory(buf, 64, mem, "");
        return sb::string(buf);
    }
}
