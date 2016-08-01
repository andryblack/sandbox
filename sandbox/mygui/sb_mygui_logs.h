//
//  sb_mygui_logs.h
//  sr-osx
//
//  Created by Andrey Kunitsyn on 24/11/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#ifndef __sr_osx__sb_mygui_logs__
#define __sr_osx__sb_mygui_logs__

#include <MyGUI_BuildConfig.h>

namespace Sandbox {
    namespace mygui {
        void log_message(const std::string& section,const MyGUI::LogLevel& level, const std::string& message);
    }
}

#endif /* defined(__sr_osx__sb_mygui_logs__) */
