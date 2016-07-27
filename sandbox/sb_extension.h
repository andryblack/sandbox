#ifndef SB_EXTENSION_H_INCLUDED
#define SB_EXTENSION_H_INCLUDED

#include <sbstd/sb_string.h>
#include <sbstd/sb_vector.h>

namespace Sandbox {
    class Application;
    
    class PlatformExtension {
    public:
        static PlatformExtension*& GetRoot();
        PlatformExtension* GetNext() { return m_next; }
        virtual void OnAppStarted(Application* app){}
        static void OnAppStartedAll(Application* app);
        virtual void OnTimer(Application* app);
        virtual bool Process(Application* app,
                             const char* method,
                             const char* args,
                             sb::string& res) = 0;
        static bool ProcessAll(Application* app,
                               const char* method,
                               const char* args,
                               sb::string& res);
        static void OnTimerAll(Application* app);
    protected:
        struct extension_response{
            sb::string method;
            sb::string args;
        };
        sb::vector<extension_response> m_pending_responses;
        void AddPendingResponse(const char* method, const char* data);
    private:
        PlatformExtension* m_next;
    protected:
        PlatformExtension();
        ~PlatformExtension();
    };
}

#endif /*SB_EXTENSION_H_INCLUDED*/