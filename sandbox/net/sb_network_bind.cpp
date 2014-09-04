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

SB_META_DECLARE_KLASS(Sandbox::NetworkRequestBase, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::NetworkRequestBase)
SB_META_PROPERTY_RO(Completed,GetCompleted)
SB_META_PROPERTY_RO(Error,GetError)
SB_META_PROPERTY_RO(ReceivedSize,GetReceivedSize)
SB_META_PROPERTY_RO(ReceivedHeaders,GetReceivedHeaders)
SB_META_PROPERTY_RO(ErrorText, GetErrorText)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::NetworkRequest, Sandbox::NetworkRequestBase)
SB_META_BEGIN_KLASS_BIND(Sandbox::NetworkRequest)
SB_META_PROPERTY_RO(Data,GetData)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::NetworkDataRequest, Sandbox::NetworkRequestBase)
SB_META_BEGIN_KLASS_BIND(Sandbox::NetworkDataRequest)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::ImageRequest, Sandbox::NetworkDataRequest)
SB_META_BEGIN_KLASS_BIND(Sandbox::ImageRequest)
SB_META_PROPERTY_RO(Image,GetImage)
SB_META_END_KLASS_BIND()

SB_META_DECLARE_KLASS(Sandbox::Network, void)
SB_META_BEGIN_KLASS_BIND(Sandbox::Network)
SB_META_METHOD(SimpleGET)
SB_META_METHOD(SimplePOST)
SB_META_METHOD(GETImage)
SB_META_END_KLASS_BIND()

namespace Sandbox {
    
    void BindNetwork(LuaVM* vm) {
        luabind::ExternClass<Sandbox::NetworkRequestBase>(vm->GetVM());
        luabind::ExternClass<Sandbox::NetworkRequest>(vm->GetVM());
        luabind::ExternClass<Sandbox::NetworkDataRequest>(vm->GetVM());
        luabind::ExternClass<Sandbox::ImageRequest>(vm->GetVM());
        luabind::ExternClass<Sandbox::Network>(vm->GetVM());
    }
}
