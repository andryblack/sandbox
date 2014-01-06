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
        
        
        class function : public impl::wrapper_base {
        public:
            function() : m_mark_destroy(false) {
            }
            virtual ~function() {}
            void MarkDestroy(){
                sb_assert(!m_mark_destroy);
                m_mark_destroy = true;
                Reset();
            }
            bool MarkedDestroy() const { return m_mark_destroy; }
            
            void PushObject(lua_State* L) {
                GetObject(L);
            }
            void ResetLuaRef() {
                Reset();
            }
            static const meta::type_info* const* get_parents();
            using impl::wrapper_base::fcall;
        private:
            bool    m_mark_destroy;
        };
        typedef sb::shared_ptr<function> function_ptr;
        
    }
}

#endif /* defined(__Sandbox__sb_luabind_function__) */
