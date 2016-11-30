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
         
         class SubSkinHalfStateInfo : public MyGUI::SubSkinStateInfo {
             MYGUI_RTTI_DERIVED( SubSkinHalfStateInfo )
         private:
             int    m_center;
             virtual void deserialization(MyGUI::xml::ElementPtr _node, MyGUI::Version _version)
             {
                 Base::deserialization(_node, _version);
                 m_center = MyGUI::utility::parseInt(_node->findAttribute("center"));
             }
         public:
             SubSkinHalfStateInfo() : m_center(0) {}
             int getCenter() const { return m_center; }
         };
         
         class SubSkinHalfHStretch : public MyGUI::SubSkin {
             MYGUI_RTTI_DERIVED( SubSkinHalfHStretch )
         public:
             SubSkinHalfHStretch();
             int m_center;
         protected:
             virtual void _setAlign(const MyGUI::IntSize& _oldsize);
             virtual void setStateData(MyGUI::IStateInfo* _data);
         };
         
         class SubSkinHalfVStretch : public MyGUI::SubSkin {
             MYGUI_RTTI_DERIVED( SubSkinHalfVStretch )
         public:
             SubSkinHalfVStretch();
             int m_center;
         protected:
             virtual void _setAlign(const MyGUI::IntSize& _oldsize);
             virtual void setStateData(MyGUI::IStateInfo* _data);
         };

     }

}


#endif /*_SB_MYGUI_HALF_STRETCH_H_INCLUDED_*/
