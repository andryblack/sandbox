#ifndef SB_DRAW_ATTRIBUTES_H_INCLUDED
#define SB_DRAW_ATTRIBUTES_H_INCLUDED

#include "meta/sb_meta.h"

namespace Sandbox {
    class DrawAttributes : public meta::object {
        SB_META_OBJECT;
    public:
        DrawAttributes() {}
    };
    typedef sb::intrusive_ptr<DrawAttributes> DrawAttributesPtr;
}


#endif /*SB_DRAW_ATTRIBUTES_H_INCLUDED*/
