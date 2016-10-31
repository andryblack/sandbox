#include "sb_time.h"

namespace Sandbox {
    
    float TimeDiff::ToFloat() const {
        return float(m_secs) + (float(m_usecs) / 1000000.0f);
    }
    
    Time Time::Now() {
        Time res;
        GHL_GetTime(&res.m_tm);
        return res;
    }
    
    TimeDiff Time::operator-(const Time &t) const {
        return TimeDiff(GHL::Int32(m_tm.secs) - GHL::Int32(t.m_tm.secs),
                        GHL::Int32(m_tm.usecs) - GHL::Int32(t.m_tm.usecs));
    }
    
}
