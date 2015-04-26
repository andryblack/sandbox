#include "sb_extension.h"

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
    
    void PlatformExtension::OnLoadAll(Application* app) {
        PlatformExtension* ext = GetRoot();
        while (ext) {
            ext->OnLoad(app);
            ext = ext->GetNext();
        }

    }
    
}