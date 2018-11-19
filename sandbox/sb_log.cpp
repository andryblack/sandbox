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
    
    bool Logger::m_verbose_enabled =
#if defined(SB_DEBUG) && !defined(SB_SILENT)
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
    
    static sb::string log_time_format = "%H:%M:%S";
    
    static void format_ts(char* buf,size_t bufsize,bool full) {
        time_t rawtime;
        struct tm * timeinfo;
        time (&rawtime);
        timeinfo = localtime (&rawtime);
        strftime(buf,bufsize,full ? "%Y-%m-%d %H:%M:%S" : log_time_format.c_str() ,timeinfo);
    }

    class GHLLogger : public GHL::Logger {
        GHL::WriteStream*   m_file;
        char m_buf[128];
        bool m_disabled;
    public:
        GHLLogger() : m_file(0),m_disabled(false) {}
        ~GHLLogger() {
            close();
        }
        virtual bool GHL_CALL AddMessage( GHL::LogLevel level, const char* message ) {
            bool res = false;
            if (m_file) {
                m_buf[0] = 0;
                format_ts(m_buf, sizeof(m_buf), false);
                size_t pos = strlen(m_buf);
                m_buf[pos] = '|';++pos;
                m_buf[pos] = level_descr[level];++pos;
                m_buf[pos] = ':';++pos;
                m_buf[pos] = 0;
                m_file->Write(reinterpret_cast<const GHL::Byte*>(m_buf), GHL::UInt32(pos));
                m_file->Write(reinterpret_cast<const GHL::Byte*>(message), GHL::UInt32(::strlen(message)));
                m_file->Write(rn, sizeof(rn));
                if (level <= GHL::LOG_LEVEL_FATAL) {
                    m_file->Flush();
                }
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
        void disable() {
            m_disabled = true;
        }
        
        bool open(GHL::VFS* vfs,const char* file) {
            close();
            m_disabled = false;
            m_file = vfs->OpenFileWrite(file);
            return m_file!=0;
        }
        bool opened() const { return m_file!=0 && !m_disabled; }
        
        void flush() {
            if (opened()) {
                m_file->Flush();
            }
        }
    };
    
    static GHLLogger file_logger;
    static sb::string g_cur_log_path;
    static sb::string g_prev_log_path;
    
    void Logger::SetTimeFormat(const char *fmt) {
        log_time_format = fmt;
    }
    
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
    void Logger::stop() {
        file_logger.disable();
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
    
    void Logger::Write(GHL::LogLevel level,const char* msg) {
        if (file_logger.opened()) {
            file_logger.AddMessage(level, msg);
        } else if (m_enable_platform_log)
        {
            GHL_Log(level, msg);
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
