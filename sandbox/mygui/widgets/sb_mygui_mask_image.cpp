#include "sb_mygui_mask_image.h"
#include "mygui/sb_mygui_render.h"
#include "sb_resources.h"
#include "MyGUI_RenderItem.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::MaskSubSkin,MyGUI::SubSkin)
SB_META_DECLARE_OBJECT(Sandbox::mygui::MaskImageWidget,MyGUI::Widget)
SB_META_DECLARE_OBJECT(Sandbox::mygui::MaskSetSubSkin,MyGUI::MainSkin)

SB_META_DECLARE_OBJECT(Sandbox::mygui::MaskSetSubSkinState,MyGUI::SubSkinStateInfo)

namespace Sandbox {
    namespace mygui {
        
        
        MaskSetSubSkinState::MaskSetSubSkinState() : m_texture(0) {}
        void MaskSetSubSkinState::deserialization(MyGUI::xml::ElementPtr _node, MyGUI::Version _version) {
            std::string texture = _node->getParent()->findAttribute("texture");
            if (texture.empty()) {
                texture = _node->getParent()->getParent()->findAttribute("texture");
            }
            // tags replacement support for Skins
            if (_version >= MyGUI::Version(1, 1))
            {
                texture = MyGUI::LanguageManager::getInstance().replaceTags(texture);
            }
            
            const MyGUI::IntSize& size = MyGUI::texture_utility::getTextureSize(texture);
            
            m_texture = MyGUI::RenderManager::getInstance().getTexture(texture);
            
            const MyGUI::IntCoord& coord = MyGUI::IntCoord::parse(_node->findAttribute("offset"));
            setRect(MyGUI::CoordConverter::convertTextureCoord(coord, size));
        }
        
        MaskSubSkin::MaskSubSkin() {
            mSeparate = true;
        }
        
        
        void MaskSubSkin::doRender(MyGUI::IRenderTarget* _target) {
            
            if (mRenderItem) {
                {
                    RenderTargetImpl* target = static_cast<RenderTargetImpl*>(_target);
                    MyGUI::Widget* widget_p = static_cast<MyGUI::Widget*>(mCroppedParent);
                    MaskImageWidget* widget = widget_p->castType<MaskImageWidget>(false);
                    
                    ShaderPtr shader;
                    TexturePtr fill_texture;
                    MyGUI::FloatRect fill_texture_uv;
                    
                    if (widget) {
                        ImagePtr    img = widget->getImage();
                        if (img && img->GetTexture()) {
                            fill_texture = img->GetTexture();
                            float itw = 1.0f / img->GetTexture()->GetWidth();
                            float ith = 1.0f / img->GetTexture()->GetHeight();
                            
                            fill_texture_uv.set( img->GetTextureX() * itw,
                                                img->GetTextureY() * ith,
                                                (img->GetTextureX() + img->GetTextureW()) * itw,
                                                (img->GetTextureY() + img->GetTextureH()) * ith);
                        }
                        shader = widget->getShader();
                    }
                    if (!fill_texture) {
                        MyGUI::ISubWidgetRect* main = widget->getSubWidgetMain();
                        if (main) {
                            MaskSetSubSkin* sub = main->castType<MaskSetSubSkin>(false);
                            if (sub) {
                                MyGUI::ITexture* tex = sub->getTexture();
                                fill_texture_uv = sub->getUVSet();
                                if (tex) {
                                    fill_texture = static_cast<TextureImpl*>(tex)->GetTexture();
                                }
                            }
                        }
                    }
                    
                    
                    if (!fill_texture) {
                        Base::doRender(_target);
                        return;
                    }
                    
                    Graphics* g = target->graphics();
                    
                    if (g) {
                        g->SetShader(shader);
                        Sandbox::Transform2d mTr = Sandbox::Transform2d();
                        
                        
                        
                        int x = mCroppedParent->getAbsoluteLeft();
                        int y = mCroppedParent->getAbsoluteTop();
                        int w = mCroppedParent->getWidth();
                        int h = mCroppedParent->getHeight();
                        
                        const MyGUI::RenderTargetInfo& info = _target->getInfo();
                        
                        x-=info.leftOffset;
                        y-=info.topOffset;
                        
                        mTr.translate(fill_texture_uv.left,fill_texture_uv.top);
                        mTr.scale(fill_texture_uv.width() / w,fill_texture_uv.height() / h);
                        mTr.translate(-x,-y);
                        
                        g->SetMask(MASK_MODE_ALPHA, fill_texture, mTr);
                        Base::doRender(_target);
                        g->SetMask(MASK_MODE_NONE, TexturePtr(), Transform2d());
                        g->SetShader(ShaderPtr());
                        
//
                    } else {
                        sb_assert(false);
                    }
                }
            }
        }
        
        MaskSetSubSkin::MaskSetSubSkin() : m_texture(0) {
            mSeparate = false;
        }
        
        
               
        void MaskSetSubSkin::doRender(MyGUI::IRenderTarget* _target) {
            
        }
        
        void MaskSetSubSkin::setStateData(MyGUI::IStateInfo* _data) {
            m_texture = _data->castType<MaskSetSubSkinState>()->get_texture();
            MyGUI::MainSkin::setStateData(_data);
        }
        
        
        MaskImageWidget::MaskImageWidget() {
            
        }
        
        MaskImageWidget::~MaskImageWidget() {
            
        }
        
        void MaskImageWidget::setShader(const ShaderPtr &s ) {
             m_shader = s;
             update_shader();
             _updateView();
        }
        
        void MaskImageWidget::update_shader() {
            if (m_shader && m_image && m_image->GetTexture()) {
                ShaderVec2UniformPtr u = m_shader->GetVec2Uniform("texture_size");
                if (u) {
                    u->SetValue(Vector2f(m_image->GetTexture()->GetWidth(),m_image->GetTexture()->GetHeight()));
                }
                u = m_shader->GetVec2Uniform("orig_texture_size");
                if (u) {
                    u->SetValue(Vector2f(m_image->GetTexture()->GetOriginalWidth(),m_image->GetTexture()->GetOriginalHeight()));
                }
            }
        }
        
        void MaskImageWidget::setPropertyOverride(const std::string& _key, const std::string& _value) {
            if (_key == "Texture") {
                setImage(RenderManager::getInstance().resources()->GetImage(_value.c_str(), false));
            } else if (_key == "Shader") {
                size_t delim_pos = _value.find(';');
                if (delim_pos!=_value.npos) {
                    sb::string vert = _value.substr(0,delim_pos);
                    sb::string frag = _value.substr(delim_pos+1);
                    setShader(RenderManager::getInstance().resources()->GetShader(vert.c_str(), frag.c_str()));
                }
            }
            else
            {
                Base::setPropertyOverride(_key, _value);
                return;
            }
            
            eventChangeProperty(this, _key, _value);
        }
        
        
        
    }
}