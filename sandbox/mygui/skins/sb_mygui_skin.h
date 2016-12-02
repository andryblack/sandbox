#ifndef SB_MYGUI_SKIN_H_INCLUDED
#define SB_MYGUI_SKIN_H_INCLUDED

#include "MyGUI_Prerequest.h"
#include "MyGUI_CommonStateInfo.h"
#include "MyGUI_SubSkin.h"
#include "MyGUI_MainSkin.h"

#include <sbstd/sb_vector.h>
#include <sbstd/sb_intrusive_ptr.h>

namespace Sandbox {
    
    class Graphics;
    class Texture;
    class Image;
    typedef sb::intrusive_ptr<Texture> TexturePtr;
    
    namespace mygui {
        
        class ColorizedSubSkinStateInfo : public MyGUI::SubSkinStateInfo {
            MYGUI_RTTI_DERIVED( ColorizedSubSkinStateInfo )
        public:
            virtual ~ColorizedSubSkinStateInfo() { }
            
            const MyGUI::Colour& getColor() const
            {
                return m_color;
            }
        private:
            virtual void deserialization(MyGUI::xml::ElementPtr _node, MyGUI::Version _version)
            {
                MyGUI::SubSkinStateInfo::deserialization(_node,_version);
                std::string text = _node->findAttribute("color");
                if (!text.empty())
                    m_color = MyGUI::Colour::parse(text);
            }
            MyGUI::Colour    m_color;
        };
        
        class ColorizedSubSkin : public MyGUI::SubSkin {
            MYGUI_RTTI_DERIVED( ColorizedSubSkin )
        public:
            virtual void _setColour(const MyGUI::Colour& _value);
            virtual void setStateData(MyGUI::IStateInfo* _data);
        private:
            MyGUI::Colour   m_color;
            MyGUI::Colour   m_state_color;
            void applyColor();
        };
        
        class CopySubSkin : public MyGUI::SubSkin {
            MYGUI_RTTI_DERIVED(CopySubSkin)
        public:
            virtual void _setUVSet(const MyGUI::FloatRect& _rect);
            virtual void _updateView();
            virtual void createDrawItem(MyGUI::ITexture* _texture,  MyGUI::ILayerNode* _node);
            virtual void doRender(MyGUI::IRenderTarget* _target);
        private:
        };
        
        class MaskSubSkin : public MyGUI::SubSkin {
            MYGUI_RTTI_DERIVED( MaskSubSkin )
        public:
            MaskSubSkin();
            virtual void doRender(MyGUI::IRenderTarget* _target);
        protected:
            virtual void setMask(Sandbox::Graphics& g,const Sandbox::Image& img);
        };
        
        class MaskSetSubSkinState : public MyGUI::SubSkinStateInfo {
            MYGUI_RTTI_DERIVED( MaskSetSubSkinState )
        public:
            MaskSetSubSkinState();
            MyGUI::ITexture* get_texture() { return m_texture; }
        protected:
            virtual void deserialization(MyGUI::xml::ElementPtr _node, MyGUI::Version _version);
        private:
            MyGUI::ITexture* m_texture;
        };
        
        class MaskSetSubSkin : public MyGUI::MainSkin {
            MYGUI_RTTI_DERIVED( MaskSetSubSkin )
        public:
            MaskSetSubSkin();
            
            virtual void doRender(MyGUI::IRenderTarget* _target);
            virtual void setStateData(MyGUI::IStateInfo* _data);
            MyGUI::ITexture* getTexture() {
                if (m_texture) {
                    return m_texture;
                }
                return MyGUI::MainSkin::getTexture();
            }
        private:
            MyGUI::ITexture* m_texture;
        };
        
        class KeepAspectSkin : public MyGUI::MainSkin {
            MYGUI_RTTI_DERIVED( KeepAspectSkin )
        public:
            KeepAspectSkin();
            virtual void _setAlign(const MyGUI::IntSize& _oldsize);
            virtual void _setUVSet(const MyGUI::FloatRect& _rect);
            virtual void setAlign(MyGUI::Align _value);
            void updateRect(float tw, float th);
        private:
            MyGUI::Align    m_align;
        };
        
        class KeepAspectMaskSubSkin : public MaskSubSkin {
            MYGUI_RTTI_DERIVED( KeepAspectMaskSubSkin )
        public:
            KeepAspectMaskSubSkin();
            virtual void setMask(Sandbox::Graphics& g,const TexturePtr& texture,const MyGUI::FloatRect& uv);
        };

        
        void register_skin();
        void unregister_skin();
        
    }
    
}


#endif
