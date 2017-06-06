#include "sb_mygui_skin.h"
#include "MyGUI_FactoryManager.h"
#include "MyGUI_SubWidgetManager.h"
#include "meta/sb_meta.h"
#include "MyGUI_RenderItem.h"
#include "MyGUI_ITexture.h"
#include "../widgets/sb_mygui_mask_image.h"
#include "../widgets/sb_mygui_mask_text.h"
#include "../widgets/sb_mygui_scene_object.h"
#include "sb_mygui_text_skin.h"
#include "sb_mygui_edit_text.h"
#include "sb_mygui_simple_text.h"
#include "sb_mygui_half_stretch.h"
#include "sb_mygui_turned_subskin.h"

#include "../sb_mygui_render.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::ColorizedSubSkinStateInfo,MyGUI::SubSkinStateInfo)

SB_META_DECLARE_OBJECT(Sandbox::mygui::ColorizedSubSkin,MyGUI::SubSkin)
SB_META_DECLARE_OBJECT(Sandbox::mygui::CopySubSkin,MyGUI::SubSkin)

SB_META_DECLARE_OBJECT(Sandbox::mygui::MaskSubSkin,MyGUI::SubSkin)
SB_META_DECLARE_OBJECT(Sandbox::mygui::MaskSetSubSkin,MyGUI::MainSkin)
SB_META_DECLARE_OBJECT(Sandbox::mygui::MaskSetSubSkinState,MyGUI::SubSkinStateInfo)
SB_META_DECLARE_OBJECT(Sandbox::mygui::KeepAspectSkin,MyGUI::MainSkin)
SB_META_DECLARE_OBJECT(Sandbox::mygui::KeepAspectMaskSubSkin,Sandbox::mygui::MaskSubSkin)

namespace Sandbox {

    namespace mygui {
        
        MYGUI_IMPL_TYPE_NAME(ColorizedSubSkinStateInfo)
        MYGUI_IMPL_TYPE_NAME(ColorizedSubSkin)
        MYGUI_IMPL_TYPE_NAME(CopySubSkin)
        MYGUI_IMPL_TYPE_NAME(MaskSubSkin)
        MYGUI_IMPL_TYPE_NAME(MaskSetSubSkin)
        MYGUI_IMPL_TYPE_NAME(MaskSetSubSkinState)
        MYGUI_IMPL_TYPE_NAME(KeepAspectSkin)
        MYGUI_IMPL_TYPE_NAME(KeepAspectMaskSubSkin)
        
        void ColorizedSubSkin::applyColor() {
            MyGUI::Colour val(m_color.red*m_state_color.red,
                              m_color.green*m_state_color.green,
                              m_color.blue*m_state_color.blue,
                              m_color.alpha*m_state_color.alpha);
            MyGUI::uint32 colour = MyGUI::texture_utility::toColourARGB(val);
            mCurrentColour = (colour & 0x00FFFFFF) | (mCurrentColour & 0xFF000000);
        }
        void ColorizedSubSkin::_setColour(const MyGUI::Colour& _value) {
            m_color = _value;
            applyColor();
            if (nullptr != mNode)
                mNode->outOfDate(mRenderItem);
        }
        void ColorizedSubSkin::setStateData(MyGUI::IStateInfo* _data) {
            MyGUI::SubSkin::setStateData(_data);
            m_state_color = _data->castType<ColorizedSubSkinStateInfo>()->getColor();
            
            applyColor();
            
            if (nullptr != mNode)
                mNode->outOfDate(mRenderItem);
        }
        
        void CopySubSkin::_setUVSet(const MyGUI::FloatRect &_rect) {
            MyGUI::FloatRect r;
            if (!mRenderItem) return;
            MyGUI::ITexture* tex = getTexture();
            if (tex) {
                int left = mCoord.left;
                int top = mCoord.top;
                r.set(float(left)/tex->getWidth(),
                      float(top)/tex->getHeight(),
                      float(left+mCoord.width)/tex->getWidth(),
                      float(top+mCoord.height)/tex->getHeight());
                Base::_setUVSet(r);
            }
        }
        
        void CopySubSkin::_updateView() {
            Base::_updateView();
            _setUVSet(MyGUI::FloatRect());
        }
        
        void CopySubSkin::createDrawItem( MyGUI::ITexture* _texture, MyGUI::ILayerNode* _node) {
            Base::createDrawItem( _texture, _node);
            _setUVSet(MyGUI::FloatRect());
        }
        
        
        void CopySubSkin::doRender(MyGUI::IRenderTarget* _target) {
            if (!mVisible || mEmptyView)
                return;
            
            _target->setTexture(getTexture());
            MyGUI::VertexQuad quad;
            
            fillQuad(quad, _target);
            
            // use premultiplied
            if (mAlign.isLeft()) {
                quad.vertex[MyGUI::VertexQuad::CornerLB].colour =
                quad.vertex[MyGUI::VertexQuad::CornerLT].colour = 0x00000000;
            }
            if (mAlign.isRight()) {
                quad.vertex[MyGUI::VertexQuad::CornerRB].colour =
                quad.vertex[MyGUI::VertexQuad::CornerRT].colour = 0x00000000;
            }
            if (mAlign.isTop()) {
                quad.vertex[MyGUI::VertexQuad::CornerLT].colour =
                quad.vertex[MyGUI::VertexQuad::CornerRT].colour = 0x00000000;
            }
            if (mAlign.isBottom()) {
                quad.vertex[MyGUI::VertexQuad::CornerLB].colour =
                quad.vertex[MyGUI::VertexQuad::CornerRB].colour = 0x00000000;
            }
            
            _target->addQuad(quad);
            
        }
        
        
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
        
        static void get_sub_image(Image& img,MaskSetSubSkin* sub) {
            MyGUI::ITexture* tex = sub->getTexture();
            if (tex) {
                MyGUI::FloatRect fill_texture_uv = sub->getUVSet();
                TexturePtr fill_texture = static_cast<TextureImpl*>(tex)->GetTexture();
                img.SetTexture(fill_texture);
                img.SetTextureRect(fill_texture_uv.left*fill_texture->GetWidth(),
                                   fill_texture_uv.top*fill_texture->GetHeight(),
                                   fill_texture_uv.width()*fill_texture->GetWidth(),
                                   fill_texture_uv.height()*fill_texture->GetHeight());
                img.SetSize(img.GetTextureW(), img.GetTextureH());
            }
        }
        
        void MaskSubSkin::doRender(MyGUI::IRenderTarget* _target) {
            if (!mVisible)
                return;
            if (mRenderItem) {
                {
                    RenderTargetImpl* target = static_cast<RenderTargetImpl*>(_target);
                    MyGUI::Widget* widget_p = static_cast<MyGUI::Widget*>(mCroppedParent);
                    MaskImageWidget* widget = widget_p->castType<MaskImageWidget>(false);
                    
                    ShaderPtr shader;
                    Image fill_image;
                    
                    if (widget) {
                        ImagePtr    img = widget->getImage();
                        if (img && img->GetTexture()) {
                            fill_image = *img;
                        }
                        shader = widget->getShader();
                    }
                    if (!fill_image.GetTexture()) {
                        MyGUI::ISubWidgetRect* main = widget_p->getSubWidgetMain();
                        if (main) {
                            MaskSetSubSkin* sub = main->castType<MaskSetSubSkin>(false);
                            if (sub) {
                                get_sub_image(fill_image,sub);
                            }
                        }
                    }
                    
                    
                    if (!fill_image.GetTexture())  {
                        Base::doRender(_target);
                        return;
                    }
                    
                    Graphics* g = target->graphics();
                    
                    if (g) {
                        if (shader && widget_p) {
                            ShaderVec2UniformPtr u = shader->GetVec2Uniform("draw_offset");
                            if (u) {
                                Sandbox::Vector2f left_top = g->GetTransform().transform(Vector2f(widget_p->getAbsoluteLeft(),widget_p->getAbsoluteTop()));
                                u->SetValue(left_top);
                                u = shader->GetVec2Uniform("draw_scale");
                                if (u) {
                                    Sandbox::Vector2f right_bottom = g->GetTransform().transform(Vector2f(widget_p->getAbsoluteLeft()+widget_p->getWidth(),widget_p->getAbsoluteTop()+widget_p->getHeight()));
                                    Sandbox::Vector2f size = right_bottom-left_top;
                                    if (size.x != 0.0f && size.y != 0.0f)
                                        u->SetValue(Vector2f(float(widget_p->getWidth())/size.x ,float(widget_p->getHeight())/size.y ));
                                }
                            }
                        }
                        Sandbox::ShaderPtr storeShader = g->GetShader();
                        Sandbox::Graphics::MaskContext mask;
                        g->StoreMask(mask);
                        g->SetShader(shader);
                        setMask(*g, fill_image);
                        Base::doRender(_target);
                        g->RestoreMask(mask);
                        g->SetShader(storeShader);
                        
                        //
                    } else {
                        sb_assert(false);
                    }
                }
            }
        }
        
        void MaskSubSkin::setMask(Sandbox::Graphics& g,const Sandbox::Image& image) {
            
            int x = mCroppedParent->getAbsoluteLeft();
            int y = mCroppedParent->getAbsoluteTop();
            int w = mCroppedParent->getWidth();
            int h = mCroppedParent->getHeight();
            
            g.SetMask(MASK_MODE_ALPHA, image, Sandbox::Rectf(x,y,w,h));

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
        
        
        KeepAspectSkin::KeepAspectSkin() {
            
        }
        
        void KeepAspectSkin::_setUVSet(const MyGUI::FloatRect& _rect) {
            updateRect(_rect.width(),_rect.height());
            Base::_setUVSet(_rect);
        }
        void KeepAspectSkin::updateRect(float tw, float th) {
            if (mTexture) {
                tw *= mTexture->getWidth();
                th *= mTexture->getHeight();
            }
            int rw = mCroppedParent->getWidth();
            int rh = mCroppedParent->getHeight();
            
            float sw = rw / tw;
            float sh = rh / th;
            float s = sw;
            if ( sh < s ) {
                s = sh;
            }
            
            int nrw = tw * s;
            int nrh = th * s;
            
            mCurrentCoord.set(0, 0, nrw, nrh);
            if (m_align.isVCenter()) {
                mCurrentCoord.top = (rh-nrh) / 2;
            } else if (m_align.isBottom()) {
                mCurrentCoord.top = rh-nrh;
            }
            if (m_align.isHCenter()) {
                mCurrentCoord.left = (rw-nrw)/2;
            } else if (m_align.isRight()) {
                mCurrentCoord.left = rw-nrw;
            }
            
            mCoord = mCurrentCoord;
            _updateView();
            
        }
        void KeepAspectSkin::_setAlign(const MyGUI::IntSize& _oldsize) {
            //mCurrentCoord.set(0, 0, mCroppedParent->getWidth(), mCroppedParent->getHeight());
            if (mRectTexture.empty()) {
                Base::_setAlign(_oldsize);
                return;
            }
            float tw = mRectTexture.width();
            float th = mRectTexture.height();
            updateRect(tw,th);
        }
        
        void KeepAspectSkin::setAlign(MyGUI::Align _value) {
            m_align = _value;
            Base::setAlign(MyGUI::Align::Stretch);
        }
        
        KeepAspectMaskSubSkin::KeepAspectMaskSubSkin() {
            mSeparate = true;
        }
        
        
        void KeepAspectMaskSubSkin::setMask(Sandbox::Graphics& g,const Sandbox::Image& image) {
            
            float x = float(mCroppedParent->getAbsoluteLeft());
            float y = float(mCroppedParent->getAbsoluteTop());
            float w = float(mCroppedParent->getWidth());
            float h = float(mCroppedParent->getHeight());
            
            float sx = w / image.GetWidth() ;
            float sy = h / image.GetHeight() ;
            
            float s = (sx > sy) ? sx : sy;
            
            float sw = image.GetWidth() * s;
            float sh = image.GetHeight() * s;
            
            float dx = (w - sw) / 2.0f;
            float dy = (h - sh) / 2.0f;
            
            g.SetMask(MASK_MODE_ALPHA, image, Sandbox::Rectf(dx+x,dy+y,sw,sh));
        }
        void register_skin() {
            MyGUI::FactoryManager& factory = MyGUI::FactoryManager::getInstance();
            
            
            const std::string& category_name = MyGUI::SubWidgetManager::getInstance().getCategoryName();
            
            factory.registerFactory<ColorizedSubSkin>(category_name);
            factory.registerFactory<CopySubSkin>(category_name);
            factory.registerFactory<MaskSubSkin>(category_name);
            factory.registerFactory<MaskSetSubSkin>(category_name);
            factory.registerFactory<ObjectSubSkin>(category_name);
            factory.registerFactory<AutoWidthText>(category_name);
            factory.registerFactory<AutoSizeText>(category_name);
            factory.registerFactory<CroppedText>(category_name);
            factory.registerFactory<MaskText>(category_name);
            factory.registerFactory<KeepAspectSkin>(category_name);
            factory.registerFactory<KeepAspectMaskSubSkin>(category_name);
            factory.registerFactory<EditText>(category_name);
            factory.registerFactory<SimpleText>(category_name);
            factory.registerFactory<SubSkinHalfHStretch>(category_name);
            factory.registerFactory<SubSkinHalfVStretch>(category_name);
            factory.registerFactory<TurnedSubSkin>(category_name);
            
            const std::string& state_category_name = MyGUI::SubWidgetManager::getInstance().getStateCategoryName();
            factory.registerFactory<ColorizedSubSkinStateInfo>(state_category_name, "ColorizedSubSkin");
            factory.registerFactory<ColorizedSubSkinStateInfo>(state_category_name, "MaskSubSkin");
            factory.registerFactory<MaskSetSubSkinState>(state_category_name, "MaskSetSubSkin");
            factory.registerFactory<MyGUI::SubSkinStateInfo>(state_category_name, "ObjectSubSkin");
            factory.registerFactory<SubSkinHalfStateInfo>(state_category_name, "SubSkinHalfHStretch");
            factory.registerFactory<SubSkinHalfStateInfo>(state_category_name, "SubSkinHalfVStretch");
            factory.registerFactory<EditTextStateInfo>(state_category_name, "AutoWidthText");
            factory.registerFactory<EditTextStateInfo>(state_category_name, "AutoSizeText");
            factory.registerFactory<EditTextStateInfo>(state_category_name, "CroppedText");
            factory.registerFactory<EditTextStateInfo>(state_category_name, "MaskText");
            factory.registerFactory<MyGUI::SubSkinStateInfo>(state_category_name, "KeepAspectSkin");
            factory.registerFactory<ColorizedSubSkinStateInfo>(state_category_name, "KeepAspectMaskSubSkin");
            factory.registerFactory<EditTextStateInfo>(state_category_name, "EditText");
            factory.registerFactory<EditTextStateInfo>(state_category_name, "SimpleText");
            factory.registerFactory<TurnedSubSkinStateInfo>(state_category_name, "TurnedSubSkin");

            
        }
        
        void unregister_skin() {
            MyGUI::FactoryManager& factory = MyGUI::FactoryManager::getInstance();
            const std::string& category_name = MyGUI::SubWidgetManager::getInstance().getCategoryName();
            
            
            factory.unregisterFactory<ColorizedSubSkin>(category_name);
            factory.unregisterFactory<CopySubSkin>(category_name);
            factory.unregisterFactory<MaskSubSkin>(category_name);
            factory.unregisterFactory<MaskSetSubSkin>(category_name);
            factory.unregisterFactory<ObjectSubSkin>(category_name);
            factory.unregisterFactory<AutoWidthText>(category_name);
            factory.unregisterFactory<AutoSizeText>(category_name);
            factory.unregisterFactory<CroppedText>(category_name);
            factory.unregisterFactory<MaskText>(category_name);
            factory.unregisterFactory<KeepAspectSkin>(category_name);
            factory.unregisterFactory<KeepAspectMaskSubSkin>(category_name);
            factory.unregisterFactory<EditText>(category_name);
            factory.unregisterFactory<SimpleText>(category_name);
            factory.unregisterFactory<SubSkinHalfHStretch>(category_name);
            factory.unregisterFactory<SubSkinHalfVStretch>(category_name);
            factory.unregisterFactory<TurnedSubSkin>(category_name);
            
            const std::string& state_category_name = MyGUI::SubWidgetManager::getInstance().getStateCategoryName();
            
            factory.unregisterFactory(state_category_name, "MaskSubSkin");
            factory.unregisterFactory(state_category_name, "MaskSetSubSkin");
            factory.unregisterFactory(state_category_name, "ObjectSubSkin");
            factory.unregisterFactory(state_category_name, "AutoWidthText");
            factory.unregisterFactory(state_category_name, "AutoSizeText");
            factory.unregisterFactory(state_category_name, "CroppedText");
            factory.unregisterFactory(state_category_name, "MaskText");
            factory.unregisterFactory(state_category_name, "KeepAspectSkin");
            factory.unregisterFactory(state_category_name, "KeepAspectMaskSubSkin");
            factory.unregisterFactory(state_category_name, "ColorizedSubSkin");
            factory.unregisterFactory(state_category_name, "EditText");
            factory.unregisterFactory(state_category_name, "SimpleText");
            factory.unregisterFactory(state_category_name, "SubSkinHalfHStretch");
            factory.unregisterFactory(state_category_name, "SubSkinHalfVStretch");
            factory.unregisterFactory(state_category_name, "TurnedSubSkin");
        }
        
    }
    
}
