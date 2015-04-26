#include <sb_application.h>

#include <AS3/AS3.h>
#include <Flash++.h>

using namespace AS3::ui;

namespace Sandbox {

	bool Application::CallExtension(const char* method, const char* args, sb::string& res ) {
		if (::strcmp(method,"get_flash_var")==0) {
			flash::display::Stage stage = internal::get_Stage();
			flash::display::LoaderInfo loaderInfo = stage->loaderInfo;
            if (loaderInfo != internal::_undefined && loaderInfo != internal::_null) {
                Object flashVars = loaderInfo->parameters;
                res = AS3::sz2stringAndFree(internal::utf8_toString(flashVars[args]));
                return true;
            }
            return false;
		} else {
			
			if (flash::external::ExternalInterface::available) {
		        try {
		            var a = internal::new_String( args );
		            var m = internal::new_String( method );
		            var r = flash::external::ExternalInterface::call(m,1,&a);
		            res = AS3::sz2stringAndFree(internal::utf8_toString(r));
		        } catch (...) {
		            res = "exception";
		            return false;
		        }
		        return true;
		    } else {
		        res = "unavailable";
		        return false;
		    }

		}
        return false;
	}

}