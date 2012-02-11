//
//  sb_log.cpp
//

#include "sb_log.h"

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
    
    
}