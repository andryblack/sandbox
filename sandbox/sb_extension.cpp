#include "sb_extension.h"
#include "sb_application.h"
#include <ghl_system.h>

namespace Sandbox {
    
    
    PlatformExtension*& PlatformExtension::GetRoot() {
        static PlatformExtension* root = 0;
        return root;
    }
    
    PlatformExtension::PlatformExtension() : m_next(GetRoot()) {
        GetRoot() = this;
    }
    
    PlatformExtension::~PlatformExtension() {
        
    }
    
    bool PlatformExtension::ProcessAll(Application* app,
                            const char* method,
                           const char* args,
                           sb::string& res) {
        PlatformExtension* ext = GetRoot();
        while (ext) {
            if (ext->Process(app,method,args,res))
                return true;
            ext = ext->GetNext();
        }
        return false;
    }
    
    void PlatformExtension::OnAppStartedAll(Application* app) {
        PlatformExtension* ext = GetRoot();
        while (ext) {
            ext->OnAppStarted(app);
            ext = ext->GetNext();
        }
    }
    void PlatformExtension::OnAppStoppedAll(Application* app) {
        PlatformExtension* ext = GetRoot();
        while (ext) {
            ext->OnAppStopped(app);
            ext = ext->GetNext();
        }
    }


    void PlatformExtension::AddPendingResponse(const char* method, const char* data) {
         GHL_GlobalLock();
        extension_response r = { method , data };
        m_pending_responses.push_back(r);
        GHL_GlobalUnlock();
    }

    void PlatformExtension::OnTimer(Application* app) {
        sb::vector<extension_response> responses;
        GHL_GlobalLock();
        responses.swap(m_pending_responses);
         GHL_GlobalUnlock();
        for (sb::vector<extension_response>::const_iterator i = responses.begin();i!=responses.end();++i) {
            app->OnExtensionResponse(i->method.c_str(),i->args.c_str());
        }
    }

     void PlatformExtension::OnTimerAll(Application* app) {
        PlatformExtension* ext = GetRoot();
        while (ext) {
            ext->OnTimer(app);
            ext = ext->GetNext();
        }
    }
    
}
