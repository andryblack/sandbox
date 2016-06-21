#ifndef SB_MYGUI_TEXT_SKIN_H_INCLUDED
#define SB_MYGUI_TEXT_SKIN_H_INCLUDED

#include "MyGUI_Prerequest.h"
#include "MyGUI_SimpleText.h"

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
            
            // метод для отрисовки себя
            virtual void doRender(MyGUI::IRenderTarget* _target);
            
            
        private:
        protected:
            virtual void updateRawData();
            float   m_scale;
           
        };

    }
}

#endif /*SB_MYGUI_TEXT_SKIN_H_INCLUDED*/