//
//  sb_mygui_logs.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 24/11/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_mygui_logs.h"
#include "MyGUI_LogManager.h"
#include "MyGUI_LogLevel.h"
#include "../sb_log.h"

namespace Sandbox {
    namespace mygui {
        
        static GHL::LogLevel convert( const MyGUI::LogLevel& level ) {
            switch (level.getValue()) {
                case MyGUI::LogLevel::Info:     return GHL::LOG_LEVEL_INFO;
                case MyGUI::LogLevel::Warning:  return GHL::LOG_LEVEL_WARNING;
                case MyGUI::LogLevel::Error:    return GHL::LOG_LEVEL_ERROR;
                case MyGUI::LogLevel::Critical: return GHL::LOG_LEVEL_FATAL;
                    
                default:
                    break;
            }
            return GHL::LOG_LEVEL_VERBOSE;
        }
        
        void log_message(const std::string& section,const MyGUI::LogLevel& level, const std::string& message) {
            Logger(convert(level),"MyGUI") << section << ": " << message;
        }
        
        
    }
}