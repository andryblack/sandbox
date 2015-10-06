#include "sb_mygui_mask_image.h"
#include "mygui/sb_mygui_render.h"
#include "sb_resources.h"
#include "MyGUI_RenderItem.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::MaskSubSkin,MyGUI::SubSkin)
SB_META_DECLARE_OBJECT(Sandbox::mygui::MaskImageWidget,MyGUI::Widget)


namespace Sandbox {
    namespace mygui {
        
        
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
                    if (!widget)
                        return;
                    
                    ImagePtr    img = widget->getImage();
                    ShaderPtr   shader = widget->getShader();
                    if (!img || !img->GetTexture()) {
                        _target->doRender(_buffer, _texture, _count);
                        return;
                    }
                    
                   
                    GHL::Render* render = target->render();
                    if (render) {
                        target->startRenderMask(texture);
                        if (shader) {
                            shader->Set(render);
                        }
                        render->SetTexture(img->GetTexture()->Present(target->resources()),0);
                        
                        int x = mCroppedParent->getAbsoluteLeft();
                        int y = mCroppedParent->getAbsoluteTop();
                        int w = mCroppedParent->getWidth();
                        int h = mCroppedParent->getHeight();
                        
                        const MyGUI::RenderTargetInfo& info = _target->getInfo();
                        
                        x-=info.leftOffset;
                        y-=info.topOffset;
                        
                        float itw = 1.0f / img->GetTexture()->GetWidth();
                        float ith = 1.0f / img->GetTexture()->GetHeight();
                        
                        m_vdata.resize(_count);
                        for (size_t i=0;i<_count;++i) {
                            GHL::Vertex2Tex& v2 = m_vdata[i];
                            GHL::Vertex& v = buffer->data()[i];
                            v2.x = v.x; v2.y = v.y; v2.z = v.z;
                            v2.color[0] = v.color[0]; v2.color[1] = v.color[1]; v2.color[2] = v.color[2]; v2.color[3] = v.color[3];
                            v2.tx = (img->GetTextureX()+img->GetTextureW()*(v.x - x)/w)*itw;
                            v2.ty = (img->GetTextureY()+img->GetTextureH()*(v.y - y)/h)*ith;
                            v2.t2x = v.tx;
                            v2.t2y = v.ty;
                        }
                        render->DrawPrimitivesFromMemory(GHL::PRIMITIVE_TYPE_TRIANGLES, GHL::VERTEX_TYPE_2_TEX, m_vdata.data(), m_vdata.size(), 0, m_vdata.size()/3);
                    }
                }
            }
        }
        
        
        MaskImageWidget::MaskImageWidget() {
            
        }
        
        MaskImageWidget::~MaskImageWidget() {
            
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