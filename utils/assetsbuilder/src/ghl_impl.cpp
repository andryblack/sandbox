#include <ghl_system.h>
#include <ghl_log.h>

#include <iostream>
#include <time.h>

#ifdef _MSC_VER
#include <windows.h>
#endif

GHL_API void GHL_CALL GHL_Log( GHL::LogLevel level,const char* message) {
    static const char* levelName[] = {
        "F:",
        "E:",
        "W:",
        "I:",
        "V:",
        "D:"
    };
#ifdef _MSC_VER
	OutputDebugStringA(message);
	OutputDebugStringA("\n");
#endif

    std::cout << levelName[level] << message << std::endl;
}