#ifndef _SB_MYGUI_HALF_STRETCH_H_INCLUDED_
#define _SB_MYGUI_HALF_STRETCH_H_INCLUDED_

#include "MyGUI_Prerequest.h"
#include "MyGUI_CommonStateInfo.h"
#include "MyGUI_SubSkin.h"
#include "MyGUI_MainSkin.h"

#include <sbstd/sb_vector.h>
#include <sbstd/sb_intrusive_ptr.h>

namespace Sandbox {
    
     namespace mygui {
         class SubSkinHalfHStretch : public MyGUI::SubSkin {
             MYGUI_RTTI_DERIVED( SubSkinHalfHStretch )
         public:
             SubSkinHalfHStretch();
         protected:
             virtual void _setAlign(const MyGUI::IntSize& _oldsize);
         };

     }

}


#endif /*_SB_MYGUI_HALF_STRETCH_H_INCLUDED_*/
