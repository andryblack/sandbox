#ifndef SB_MYGUI_TEXT_SKIN_H_INCLUDED
#define SB_MYGUI_TEXT_SKIN_H_INCLUDED

#include "MyGUI_Prerequest.h"
#include "MyGUI_SimpleText.h"

#include <sbstd/sb_string.h>

namespace Sandbox {
    
    namespace mygui {

        class AutoSizeText :
            public MyGUI::EditText
        {
            MYGUI_RTTI_DERIVED( AutoSizeText )
            
        public:
            AutoSizeText();
            virtual ~AutoSizeText();
            
            virtual void setViewOffset(const MyGUI::IntPoint& _point);
            
            virtual void doRender(MyGUI::IRenderTarget* _target);
            virtual void renderPass(MyGUI::IRenderTarget* _target,size_t pass);
            
        private:
        protected:
            virtual void updateRawData();
            float   m_scale;
           
        };
        
        class MaskText : public AutoSizeText {
             MYGUI_RTTI_DERIVED( MaskText )
        public:
            MaskText();
            virtual ~MaskText();
            
            // метод для отрисовки себя
            
            virtual void renderPass(MyGUI::IRenderTarget* _target,size_t pass);
            
        };

    }
}

#endif /*SB_MYGUI_TEXT_SKIN_H_INCLUDED*/