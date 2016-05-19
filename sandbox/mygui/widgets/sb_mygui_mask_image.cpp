#include "sb_mygui_mask_image.h"
#include "mygui/sb_mygui_render.h"
#include "sb_resources.h"
#include "MyGUI_RenderItem.h"

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
            m_texture = MyGUI::RenderManager::getInstance().getTexture(texture);
            
            const MyGUI::IntSize& size = MyGUI::texture_utility::getTextureSize(texture);
            const MyGUI::IntCoord& coord = MyGUI::IntCoord::parse(_node->findAttribute("offset"));
            setRect(MyGUI::CoordConverter::convertTextureCoord(coord, size));
        }
        
        MaskSubSkin::MaskSubSkin() {
            mSeparate = true;
        }
        
        
        void MaskSubSkin::doManualRender(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count) {
            if (mRenderItem) {
                MyGUI::IRenderTarget* _target = mRenderItem->getRenderTarget();
                if (_target) {
                    VertexBufferImpl* buffer = static_cast<VertexBufferImpl*>(_buffer);
                    RenderTargetImpl* target = static_cast<RenderTargetImpl*>(_target);
                    TextureImpl* texture = static_cast<TextureImpl*>(_texture);
                    MyGUI::Widget* widget_p = static_cast<MyGUI::Widget*>(mCroppedParent);
                    MaskImageWidget* widget = widget_p->castType<MaskImageWidget>(false);
                    
                    ShaderPtr shader;
                    const GHL::Texture* fill_texture = 0;
                    MyGUI::FloatRect fill_texture_uv;
                    
                    if (widget) {
                        ImagePtr    img = widget->getImage();
                        if (img && img->GetTexture()) {
                            fill_texture = img->GetTexture()->Present(target->resources());
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
                                    fill_texture = static_cast<TextureImpl*>(tex)->Present();
                                }
                            }
                        }
                    }
                    
                    
                    if (!fill_texture) {
                        _target->doRender(_buffer, _texture, _count);
                        return;
                    }
                                        
                    target->endRenderObject();
                    GHL::Render* render = target->render();
                    
                    if (render) {
                        target->startRenderMask(texture);
                        if (shader) {
                            shader->Set(render);
                        } else {
                            render->SetShader(0);
                        }
                        render->SetTexture(fill_texture,0);
                        
                        int x = mCroppedParent->getAbsoluteLeft();
                        int y = mCroppedParent->getAbsoluteTop();
                        int w = mCroppedParent->getWidth();
                        int h = mCroppedParent->getHeight();
                        
                        const MyGUI::RenderTargetInfo& info = _target->getInfo();
                        
                        x-=info.leftOffset;
                        y-=info.topOffset;
                        
                        m_vdata.resize(_count);
                        for (size_t i=0;i<_count;++i) {
                            GHL::Vertex2Tex& v2 = m_vdata[i];
                            GHL::Vertex& v = buffer->data()[i];
                            v2.x = v.x; v2.y = v.y; v2.z = v.z;
                            v2.color[0] = v.color[0]; v2.color[1] = v.color[1]; v2.color[2] = v.color[2]; v2.color[3] = v.color[3];
                            v2.tx = (fill_texture_uv.left+fill_texture_uv.width()*(v.x - x)/w);
                            v2.ty = (fill_texture_uv.top+fill_texture_uv.height()*(v.y - y)/h);
                            v2.t2x = v.tx;
                            v2.t2y = v.ty;
                        }
                        render->DrawPrimitivesFromMemory(GHL::PRIMITIVE_TYPE_TRIANGLES, GHL::VERTEX_TYPE_2_TEX, m_vdata.data(), m_vdata.size(), 0, m_vdata.size()/3);
                    } else {
                        sb_assert(false);
                    }
                }
            }
        }
        
        MaskSetSubSkin::MaskSetSubSkin() : m_texture(0) {
            mSeparate = false;
        }
        
        
        void MaskSetSubSkin::doManualRender(MyGUI::IVertexBuffer* _buffer, MyGUI::ITexture* _texture, size_t _count) {
            
        }
        
        void MaskSetSubSkin::doRender() {
            
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