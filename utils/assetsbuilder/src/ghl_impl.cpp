#include <ghl_system.h>
#include <ghl_log.h>

#include <iostream>
#include <time.h>


GHL_API void GHL_CALL GHL_Log( GHL::LogLevel level,const char* message) {
    static const char* levelName[] = {
        "F:",
        "E:",
        "W:",
        "I:",
        "V:",
        "D:"
    };
    
    std::cout << levelName[level] << message << std::endl;
}