//
//  sb_log.h
//

#ifndef SB_LOG_H_INCLUDED
#define SB_LOG_H_INCLUDED

#include <sstream>
#include <ghl_log.h>

namespace Sandbox {
    
    class Logger {
    public:
        explicit Logger( GHL::LogLevel level,const char* module = 0 );
        ~Logger();
        template <class T>
        Logger& operator << (const T& val) {
            m_stream << val;
            return *this;
        }
    private:
        Logger( const Logger& );
        Logger& operator = (const Logger&);
        const char*     m_module;
        GHL::LogLevel       m_level;
        std::stringstream   m_stream;
    };
    
    class EmptyLogger {
        template <class T>
        EmptyLogger& operator << (const T& val) {
            return *this;
        }
    };
    
    class LogError : public Logger {
    public:
        LogError( const char* module) : Logger( GHL::LOG_LEVEL_ERROR,module) {
            
        }
    };
    
    class LogWarning : public Logger {
    public:
        LogWarning( const char* module) : Logger( GHL::LOG_LEVEL_WARNING,module) {
            
        }
    };

    class LogInfo : public Logger {
    public:
        LogInfo( const char* module) : Logger( GHL::LOG_LEVEL_INFO,module) {
            
        }
    };
    
    class LogVerbose : public Logger {
    public:
        LogVerbose( const char* module) : Logger( GHL::LOG_LEVEL_VERBOSE,module) {
            
        }
    };
    
    class LogDebug : public Logger {
    public:
        LogDebug( const char* module) : Logger( GHL::LOG_LEVEL_DEBUG,module) {
            
        }
    };
    
    
}

#endif /*SB_LOG_H_INCLUDED*/
