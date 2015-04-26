#ifndef SB_EXTENSION_H_INCLUDED
#define SB_EXTENSION_H_INCLUDED

#include <sbstd/sb_string.h>

namespace Sandbox {
    class Application;
    
    class PlatformExtension {
    public:
        static PlatformExtension*& GetRoot();
        PlatformExtension* GetNext() { return m_next; }
        virtual void OnLoad(Application* app) {}
        static void OnLoadAll(Application* app);
        virtual bool Process(Application* app,
                             const char* method,
                             const char* args,
                             sb::string& res) = 0;
        static bool ProcessAll(Application* app,
                               const char* method,
                               const char* args,
                               sb::string& res);
    private:
        PlatformExtension* m_next;
    protected:
        PlatformExtension();
        ~PlatformExtension();
    };
}

#endif /*SB_EXTENSION_H_INCLUDED*/