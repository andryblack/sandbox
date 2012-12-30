//
//  sb_log.cpp
//

#include "sb_log.h"

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

namespace Sandbox {
    
    static const char* level_descr[] = {
        ": FATAL :",
        ": ERROR :",
        ":WARNING:",
        ": INFO  :",
        ":VERBOSE:",
        ": DEBUG :"
    };
    
    Logger::Logger( GHL::LogLevel level , const char* module) : m_level( level ), m_module(module){
        m_stream << "[" << (m_module ? m_module : "Sandbox") << "]" << level_descr[m_level];
    }
    
    Logger::~Logger() {
        GHL_Log(m_level, m_stream.str().c_str());
    }
    
    void format_memory( char* buf, size_t size, GHL::UInt32 mem,const char* caption ) {
        if ( mem > 1024*1024 ) {
            ::snprintf(buf, size, "%s%0.2fM", caption,float(mem)/(1024*1024));
        } else if ( mem > 1024 ) {
            ::snprintf(buf, size, "%s%0.2fK", caption,float(mem)/(1024));
        }
        else {
            ::snprintf(buf, size, "%s%0.2fb", caption,float(mem));
        }
    }
    
    sb::string format_memory( GHL::UInt32 mem ) {
        char buf[64];
        format_memory(buf, 64, mem, "");
        return sb::string(buf);
    }
}