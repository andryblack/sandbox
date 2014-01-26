//
//  sb_luabind_function.h
//  Sandbox
//
//  Created by Andrey Kunitsyn on 06/01/14.
//
//

#ifndef __Sandbox__sb_luabind_function__
#define __Sandbox__sb_luabind_function__

#include "sb_luabind_stack.h"
#include "sb_luabind_ref.h"
#include <sbstd/sb_traits.h>
#include "impl/sb_luabind_wrapper_base.h"

namespace Sandbox {
    namespace luabind {
        
        
        class function : public impl::function_wrapper_base {
        public:
            using impl::function_wrapper_base::call;
        private:
        };
        typedef sb::shared_ptr<function> function_ptr;
        
        

    }
}

#endif /* defined(__Sandbox__sb_luabind_function__) */
