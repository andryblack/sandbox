#include "sb_event.h"

namespace Sandbox {
    
    static const sb::string empty_string;
    
    void Event::SetString(const sb::string& key,const sb::string& val) {
        m_strings[key] = val;
    }
    const sb::string& Event::GetString(const sb::string& key) const {
        sb::map<sb::string, sb::string>::const_iterator it = m_strings.find(key);
        return it == m_strings.end() ? empty_string : it->second;
    }
    void Event::SetInt(const sb::string& key,int v) {
        m_ints[key] = v;
    }
    int Event::GetInt(const sb::string& key) const {
        sb::map<sb::string, int>::const_iterator it = m_ints.find(key);
        return it == m_ints.end() ? 0 : it->second;
    }
    
}
