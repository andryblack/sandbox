/*
 *  Copyright 2016 andryblack. All rights reserved.
 *
 */

#ifndef SB_EVENT_H_INCLUDED
#define SB_EVENT_H_INCLUDED

#include <sbstd/sb_intrusive_ptr.h>
#include <sbstd/sb_map.h>
#include <sbstd/sb_string.h>

namespace Sandbox {
    
    class Event : public sb::ref_countered_base_not_copyable {
    private:
        sb::map<sb::string,sb::string> m_strings;
        sb::map<sb::string,int> m_ints;
    public:
        void SetString(const sb::string& key,const sb::string& val);
        const sb::string& GetString(const sb::string& key) const;
        void SetInt(const sb::string& ket,int v);
        int GetInt(const sb::string& key) const;
    };
    typedef sb::intrusive_ptr<Event> EventPtr;
    
}

#endif /*SB_EVENT_H_INCLUDED*/
