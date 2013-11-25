#ifndef SB_MYGUI_CONFIG_H_INCLUDED
#define SB_MYGUI_CONFIG_H_INCLUDED

#define MYGUI_STATIC
#define MYGUI_DONT_USE_DYNLIB
#define MYGUI_DONT_USE_OBSOLETE

#include <string>

namespace MyGUI
{
    struct LogLevel;
}
namespace Sandbox {
    namespace mygui {
        void log_message(const std::string& section,const MyGUI::LogLevel& level, const std::string& message);
        
    }
}
        
#define MYGUI_LOGGING(section, level, text) \
    Sandbox::mygui::log_message(section,MyGUI::LogLevel::level,MyGUI::LogStream()<<text<<MyGUI::LogStream::End())

#endif /*SB_MYGUI_CONFIG_H_INCLUDED*/