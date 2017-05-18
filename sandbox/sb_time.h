#ifndef SB_TIME_H_INCLUDED
#define SB_TIME_H_INCLUDED

#include <ghl_time.h>

namespace Sandbox {
    
    class TimeDiff {
    private:
        GHL::Int32   m_secs;
        GHL::Int32   m_usecs;
    public:
        explicit TimeDiff(GHL::Int32 dsec) : m_secs(dsec),m_usecs(0) {}
        explicit TimeDiff(GHL::Int32 dsec,GHL::Int32 dusec) : m_secs(dsec),m_usecs(dusec) {}
        GHL::UInt32 msec() const { return m_secs*1000+m_usecs/1000; }
        float ToFloat() const;
    };
    
    class Time {
    private:
        GHL::TimeValue m_tm;
    public:
        static Time Now();
        TimeDiff operator - (const Time& t) const;
        GHL::UInt32 sec() const { return m_tm.secs; }
    };
    
}

#endif /*SB_TIME_H_INCLUDED*/
