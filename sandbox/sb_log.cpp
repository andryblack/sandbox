//
//  sb_log.cpp
//

#include "sb_log.h"

#include <sbstd/sb_platform.h>
#include <ghl_vfs.h>
#include "meta/sb_meta.h"
#include <ctime>



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
    
    static const char level_descr[] = {
        'F',
        'E',
        'W',
        'I',
        'V',
        'D'
    };
    
    static void format_ts(char* buf,size_t bufsize,bool full) {
        time_t rawtime;
        struct tm * timeinfo;
        time (&rawtime);
        timeinfo = localtime (&rawtime);
        strftime(buf,bufsize,full ? "%Y-%m-%d %H:%M:%S" : "%H:%M:%S" ,timeinfo);
    }

    class GHLLogger : public GHL::Logger {
        GHL::WriteStream*   m_file;
    public:
        GHLLogger() : m_file(0) {}
        ~GHLLogger() {
            close();
        }
        virtual bool GHL_CALL AddMessage( GHL::LogLevel level, const char* message ) {
            bool res = false;
            if (m_file) {
                char buf[128] = {0};
                format_ts(buf, sizeof(buf), false);
                size_t pos = strlen(buf);
                buf[pos] = ':';++pos;
                buf[pos] = level_descr[level];++pos;
                buf[pos] = ':';++pos;
                buf[pos] = 0;
                m_file->Write(reinterpret_cast<const GHL::Byte*>(buf), pos);
                m_file->Write(reinterpret_cast<const GHL::Byte*>(message), ::strlen(message));
                m_file->Write(rn, sizeof(rn));
                res = true;
            }
            if (Sandbox::Logger::GetPlatformLogEnabled()) {
                GHL_Log(level, message);
                res = true;
            }
            return res;
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
        
        void flush() {
            if (opened()) {
                m_file->Flush();
            }
        }
    };
    
    static GHLLogger file_logger;
    static sb::string g_cur_log_path;
    static sb::string g_prev_log_path;
    
    
    
    void Logger::StartSession(GHL::VFS* vfs) {
#if !defined(GHL_PLATFORM_EMSCRIPTEN) && !defined(GHL_PLATFORM_FLASH)
        sb::string path = vfs->GetDir(GHL::DIR_TYPE_USER_PROFILE);
        if (!path.empty()) {
            if (path[path.length()-1]!='/') {
                path += "/";
            }
        }
        file_logger.close();
        g_prev_log_path =path + prev_log;
        g_cur_log_path = path + curr_log;
        if (vfs->IsFileExists(g_prev_log_path.c_str())) {
            GHL_Log(GHL::LOG_LEVEL_INFO, (sb::string("remove ") + g_prev_log_path).c_str() );
            vfs->DoRemoveFile(g_prev_log_path.c_str());
        }
        if (!vfs->DoRenameFile(g_cur_log_path.c_str(),g_prev_log_path.c_str())) {
            GHL_Log(GHL::LOG_LEVEL_ERROR, (sb::string("failed rename ") + g_cur_log_path).c_str() );
        }
        if (file_logger.open(vfs,g_cur_log_path.c_str())) {
            GHL_Log(GHL::LOG_LEVEL_INFO, (sb::string("start writing ") + g_cur_log_path).c_str() );
            GHL_SetLogger(&file_logger);
        }
#endif
        char time_format[128];
        format_ts(time_format,sizeof(time_format),true);
        Sandbox::LogInfo("SB") << "start session at " << time_format;
    }
    
    void Logger::flush() {
        file_logger.flush();
    }
    
    sb::string Logger::GetCurrentPath() {
        return g_cur_log_path;
    }
    sb::string Logger::GetPrevPath() {
        return g_prev_log_path;
    }
    
    Logger::Logger( GHL::LogLevel level , const char* module) :  m_module(module?module:"sbx"),m_level( level ){
        m_stream << "[" << m_module << "] ";
    }
    
    Logger::~Logger() {
        if (m_module) {
            if (file_logger.opened()) {
                file_logger.AddMessage(m_level, m_stream.str().c_str());
            } else if (m_enable_platform_log)
            {
                GHL_Log(m_level, m_stream.str().c_str());
            }
        }
    }
    
    void Logger::Discard() {
        m_module = 0;
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
    
    LogProfileTime::LogProfileTime(const char* module,const char* name) : Sandbox::Logger(GHL::LOG_LEVEL_VERBOSE,module),
        m_start(Time::Now()){
        *this << name << ":";
    }
    LogProfileTime::~LogProfileTime() {
        TimeDiff td = Time::Now() - m_start;
        if (td.msec() > 100) {
            *this << td.ToFloat() << "s";
        } else {
            Discard();
        }
    }
}
