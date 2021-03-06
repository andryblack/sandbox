#ifndef SB_MYGUI_TEXT_SKIN_H_INCLUDED
#define SB_MYGUI_TEXT_SKIN_H_INCLUDED

#include "MyGUI_Prerequest.h"
#include "sb_mygui_edit_text.h"

#include <sbstd/sb_string.h>

namespace Sandbox {
    
    namespace mygui {

        class CroppedText :
        public EditText
        {
            MYGUI_RTTI_DERIVED( CroppedText )
            
        public:
            CroppedText();
            virtual ~CroppedText();
            
            virtual void doRender(MyGUI::IRenderTarget* _target);
        };
        
        class AutoWidthText : public EditText{
            MYGUI_RTTI_DERIVED( AutoWidthText )
        public:
            AutoWidthText();
            virtual void setViewOffset(const MyGUI::IntPoint& _point);
            virtual void _setAlign(const MyGUI::IntSize& _oldsize);
            
            virtual void doRender(MyGUI::IRenderTarget* _target);
        protected:
            virtual void updateRawData();
            void updateOffset(const MyGUI::IntSize& viewSize);
             float   m_scale;
        };
        
        class AutoSizeText :
            public AutoWidthText
        {
            MYGUI_RTTI_DERIVED( AutoSizeText )
            
        public:
            AutoSizeText();
            virtual ~AutoSizeText();
            
            
            
            
        private:
        protected:
            virtual void updateRawData();
           
           
        };
        
        
        
        class MaskText : public AutoSizeText {
             MYGUI_RTTI_DERIVED( MaskText )
        public:
            MaskText();
            virtual ~MaskText();
        protected:
            virtual bool BeginPass(Graphics& g,const FontPass& pass) const;
            virtual void EndPass(Graphics& g,const FontPass& pass) const;
        };

    }
}

#endif /*SB_MYGUI_TEXT_SKIN_H_INCLUDED*/
