//
//  sb_network_bind.cpp
//  Sandbox
//
//  Created by Andrey Kunitsyn on 01/09/14.
//
//

#include "sb_network_bind.h"
#include "sb_network.h"
#include "luabind/sb_luabind.h"
#include "sb_lua.h"

SB_META_BEGIN_KLASS_BIND(Sandbox::NetworkRequestBase)
SB_META_PROPERTY_RO(Completed,GetCompleted)
SB_META_PROPERTY_RO(Error,GetError)
SB_META_PROPERTY_RO(StatusCode, GetStatusCode)
SB_META_PROPERTY_RO(ReceivedSize,GetReceivedSize)
SB_META_PROPERTY_RO(ReceivedHeaders,GetReceivedHeaders)
SB_META_PROPERTY_RO(ErrorText, GetErrorText)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::NetworkRequest)
SB_META_PROPERTY_RO(Data,GetData)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::NetworkDataRequest)
SB_META_END_KLASS_BIND()

SB_META_BEGIN_KLASS_BIND(Sandbox::NetworkFileRequest)
SB_META_END_KLASS_BIND()

#ifndef SB_NO_RESOURCES
SB_META_BEGIN_KLASS_BIND(Sandbox::ImageRequest)
SB_META_PROPERTY_RO(Image,GetImage)
SB_META_END_KLASS_BIND()
#endif

SB_META_BEGIN_KLASS_BIND(Sandbox::NetworkPostData)
SB_META_END_KLASS_BIND()
SB_META_BEGIN_KLASS_BIND(Sandbox::NetworkMultipartFormData)
SB_META_CONSTRUCTOR(())
SB_META_METHOD(AddFile)
SB_META_METHOD(AddFormField)
SB_META_METHOD(Close)
SB_META_END_KLASS_BIND()


SB_META_BEGIN_KLASS_BIND(Sandbox::Network)
SB_META_METHOD(SimpleGET)
SB_META_METHOD(SimplePOST)
SB_META_METHOD(GETFile)
SB_META_METHOD(POST)
SB_META_METHOD(SetCookie);
#ifndef SB_NO_RESOURCES
SB_META_METHOD(GETImage)
#endif
SB_META_END_KLASS_BIND()

namespace Sandbox {
    
    void BindNetwork(LuaVM* vm) {
        luabind::ExternClass<Sandbox::NetworkRequestBase>(vm->GetVM());
        luabind::ExternClass<Sandbox::NetworkRequest>(vm->GetVM());
        luabind::ExternClass<Sandbox::NetworkDataRequest>(vm->GetVM());
        luabind::ExternClass<Sandbox::NetworkFileRequest>(vm->GetVM());
        luabind::ExternClass<Sandbox::NetworkPostData>(vm->GetVM());
        luabind::Class<Sandbox::NetworkMultipartFormData>(vm->GetVM());
#ifndef SB_NO_RESOURCES
        luabind::ExternClass<Sandbox::ImageRequest>(vm->GetVM());
#endif
        luabind::ExternClass<Sandbox::Network>(vm->GetVM());
    }
}
