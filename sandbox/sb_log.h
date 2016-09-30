//
//  sb_log.h
//

#ifndef SB_LOG_H_INCLUDED
#define SB_LOG_H_INCLUDED

#include <sstream>
#include <ghl_log.h>
#include <ghl_types.h>
#include <sbstd/sb_string.h>

namespace GHL {
    struct VFS;
}

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
        static void StartSession(GHL::VFS* vfs);
        static void SetPlatformLogEnabled(bool e) { m_enable_platform_log = e; }
        static bool GetPlatformLogEnabled() { return m_enable_platform_log; }
        static void flush();
        static sb::string GetCurrentPath();
        static sb::string GetPrevPath();
    private:
        Logger( const Logger& );
        Logger& operator = (const Logger&);
        const char*     m_module;
        GHL::LogLevel       m_level;
        std::stringstream   m_stream;
        static bool m_enable_platform_log;
    };
    
    class EmptyLogger {
    public:
		EmptyLogger() {}
        explicit EmptyLogger( const char* ) {}
        template <class T>
        EmptyLogger& operator << (const T& ) {
            return *this;
        }
    };
    
    class LogError : public Logger {
    public:
        LogError( const char* module = 0) : Logger( GHL::LOG_LEVEL_ERROR,module) {
            
        }
    };
    
    class LogWarning : public Logger {
    public:
        LogWarning( const char* module = 0) : Logger( GHL::LOG_LEVEL_WARNING,module) {
            
        }
    };

    class LogInfo : public Logger {
    public:
        LogInfo( const char* module = 0) : Logger( GHL::LOG_LEVEL_INFO,module) {
            
        }
    };
    
#define SB_LOGE( X ) do { ::Sandbox::LogError(MODULE) << X ; } while(false)
#define SB_LOGW( X ) do { ::Sandbox::LogWarning(MODULE) << X ; } while(false)
#define SB_LOGI( X ) do { ::Sandbox::LogInfo(MODULE) << X ; } while(false)
    
#if defined(SB_DEBUG) && !defined(SB_SILENT)
    
    class LogVerbose : public Logger {
    public:
        LogVerbose( const char* module = 0) : Logger( GHL::LOG_LEVEL_VERBOSE,module) {
            
        }
    };
    
    class LogDebug : public Logger {
    public:
        LogDebug( const char* module = 0) : Logger( GHL::LOG_LEVEL_DEBUG,module) {
            
        }
    };
#define SB_LOGV( X ) do { ::Sandbox::LogVerbose(MODULE) << X ; } while(false)
#define SB_LOGD( X ) do { ::Sandbox::LogDebug(MODULE) << X ; } while(false)

#else
    typedef EmptyLogger LogVerbose;
    typedef EmptyLogger LogDebug;

#define SB_LOGV( X ) do { } while(false)
#define SB_LOGD( X ) do { } while(false)

#endif
    
    void format_memory( char* buf, size_t size, size_t mem,const char* caption );
    sb::string format_memory( size_t mem );
}

#endif /*SB_LOG_H_INCLUDED*/
