#include "sb_mygui_text_skin.h"
#include "meta/sb_meta.h"
#include "MyGUI_RenderItem.h"
#include "../widgets/sb_mygui_mask_text.h"
#include "mygui/sb_mygui_render.h"
#include "sb_resources.h"
#include "sb_graphics.h"
#include "sb_mygui_skin.h"
#include "MyGUI_FontData.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::AutoWidthText,Sandbox::mygui::EditText)
SB_META_DECLARE_OBJECT(Sandbox::mygui::AutoSizeText,Sandbox::mygui::AutoWidthText)
SB_META_DECLARE_OBJECT(Sandbox::mygui::MaskText,Sandbox::mygui::AutoSizeText)
SB_META_DECLARE_OBJECT(Sandbox::mygui::CroppedText,Sandbox::mygui::EditText)


namespace Sandbox {
    
    namespace mygui {
        
        MYGUI_IMPL_TYPE_NAME(AutoWidthText)
        MYGUI_IMPL_TYPE_NAME(AutoSizeText)
        MYGUI_IMPL_TYPE_NAME(MaskText)
        MYGUI_IMPL_TYPE_NAME(CroppedText)
        
        AutoWidthText::AutoWidthText() {
            mIsAddCursorWidth = false;
            m_scale = 1.0f;
        }
        
        AutoSizeText::AutoSizeText() {
            
        }
        AutoSizeText::~AutoSizeText() {
            
        }
        
        void AutoWidthText::setViewOffset(const MyGUI::IntPoint& _point) {
            
        }
        
        void AutoWidthText::_setAlign(const MyGUI::IntSize& _oldsize) {
            Base::_setAlign(_oldsize);
            if (mCoord.size()!=_oldsize) {
                mTextOutDate = true;
            }
        }
        
        void AutoSizeText::updateRawData() {
            AutoWidthText::updateRawData();
            if (nullptr == mFont)
                return;
            MyGUI::IntSize size = mTextView.getViewSize();
            if (mWordWrap || mTextView.getData().data.size() > 1) {
                while ((size.height*m_scale) > mCoord.height) {
                    m_scale -= 0.125f/4;
                    if (m_scale < 0.125f) {
                        m_scale = 0.125f;
                    }
                    if (mWordWrap) {
                        int width = mCoord.width / m_scale;
                        // обрезать слова нужно по шарине, которую мы реально используем
                        if (mIsAddCursorWidth)
                            width -= 2;
                        mTextView.update(mCaption, mFont, mTextAlign, width);
                        size = mTextView.getViewSize();
                        size.width *= GetFontScale();
                        size.height *= GetFontScale();
                    }
                    if (m_scale <= 0.125f) {
                        m_scale = 0.125f;
                        break;
                    }
                }
            }

        }
        
        void AutoWidthText::updateRawData() {
            if (nullptr == mFont)
                return;
            // сбрасывам флаги
            mTextOutDate = false;
            m_scale = 1.0f;
            
            int width = -1;
            if (mWordWrap)
            {
                width = mCoord.width / m_scale;
                // обрезать слова нужно по шарине, которую мы реально используем
                if (mIsAddCursorWidth)
                    width -= 2;
            }
            
            mTextView.update(mCaption, mFont, mTextAlign, width);
            
            MyGUI::IntSize size = mTextView.getViewSize();
            
            size.width *= GetFontScale();
            size.height *= GetFontScale();
            
            float hscale = 1.0f;
            if (size.width > mCoord.width) {
                hscale = float(mCoord.width) / float(size.width);
                if (hscale < m_scale) {
                    m_scale = hscale;
                }
            }
            
            if (mTextAlign.isRight())
                mViewOffset.left = - (mCoord.width - float(size.width)*m_scale);
            else if (mTextAlign.isHCenter())
                mViewOffset.left = - ((mCoord.width - float(size.width)*m_scale) / 2);
            else
                mViewOffset.left = 0;
            
            if (mTextAlign.isBottom())
                mViewOffset.top = - (mCoord.height - float(size.height)*m_scale);
            else if (mTextAlign.isVCenter())
                mViewOffset.top = - ((mCoord.height - float(size.height)*m_scale) / 2);
            else
                mViewOffset.top = 0;

        }
        
        void AutoWidthText::doRender(MyGUI::IRenderTarget* _target) {
                        
            if (nullptr == mFont)
                return;
            if (!mVisible || mEmptyView)
                return;
            
            if (mTextOutDate) {
                updateRawData();
            }
            
            float scale = m_scale * GetFontScale();
            
            
            const TextData& textViewData = mTextView.getData();
            
            Graphics& g = *(static_cast<RenderManager*>(MyGUI::RenderManager::getInstancePtr())->graphics());
            Color c = g.GetColor();
            g.SetColor(c * Color(1.0f,1.0f,1.0f,mAlpha));
            Transform2d tr = g.GetTransform();
            g.SetTransform(tr.translated(mCroppedParent->getAbsoluteLeft()-mViewOffset.left + mCoord.left,
                                         mCroppedParent->getAbsoluteTop() -mViewOffset.top + mCoord.top )
                           .scale(scale));
            mFont->Draw(g, &m_attributes,  textViewData);
            g.SetColor(c);
            g.SetTransform(tr);
        }
        
        
        CroppedText::CroppedText() {
            
        }
        
        CroppedText::~CroppedText() {
            
        }
        
        
        void CroppedText::doRender(MyGUI::IRenderTarget *_target) {
            if (nullptr == mFont || !mVisible || mEmptyView)
                return;
            
            if (mRenderItem->getCurrentUpdate() || mTextOutDate)
                updateRawData();
            
            const TextData& textViewData = mTextView.getData();
            
            Graphics& g = *(static_cast<RenderManager*>(MyGUI::RenderManager::getInstancePtr())->graphics());
            
            Color c = g.GetColor();
            g.SetColor(c * Color(1.0f,1.0f,1.0f,mAlpha));
            
            float scale = GetFontScale();
            
            float top = (-mViewOffset.top + mCoord.top) + mCroppedParent->getAbsoluteTop();
            float left = ( - mViewOffset.left + mCoord.left) + mCroppedParent->getAbsoluteLeft();
            
            Transform2d tr = g.GetTransform();
            g.SetTransform(tr.translated(left, top).scale(scale));
            
            mFont->DrawCropped(g, &m_attributes,
                               Rectf(mCurrentCoord.left + mViewOffset.left,
                                     mCurrentCoord.top  + mViewOffset.top,
                                     mCurrentCoord.width,
                                     mCurrentCoord.height),
                               textViewData);
            
            if (mVisibleCursor)
            {
                MyGUI::IntPoint point = mTextView.getCursorPoint(mCursorPosition);
                const FontData::Glypth* cursorGlyph = mFont->GetGlyph(static_cast<MyGUI::Char>(MyGUI::FontCodeType::Cursor));
                if (cursorGlyph) {
                    g.DrawImage(cursorGlyph->img, &m_attributes,
                                point.left,
                                point.top);
                }
            }
            g.SetTransform(tr);
            
            g.SetColor(c);

        }
        
        
        MaskText::MaskText() {
            
        }
        
        MaskText::~MaskText() {
            
        }
        
        bool MaskText::BeginPass(Graphics& g,const FontPass& pass) const {
            Base::BeginPass(g, pass);
             if (mRenderItem) {
                 MyGUI::Widget* widget_p = static_cast<MyGUI::Widget*>(mCroppedParent);
                 MaskTextWidget* widget = widget_p->castType<MaskTextWidget>(false);
                 if (!widget) {
                     return true;
                 }
                 ShaderPtr shader;
                 TexturePtr fill_texture;
                 MyGUI::FloatRect fill_texture_uv;
                 
                 ImagePtr    img = widget->getPassImage(pass.GetName());
                 if (img && img->GetTexture()) {
                     fill_texture = img->GetTexture();
                     float itw = 1.0f / img->GetTexture()->GetWidth();
                     float ith = 1.0f / img->GetTexture()->GetHeight();
                     
                     fill_texture_uv.set( img->GetTextureX() * itw,
                                         img->GetTextureY() * ith,
                                         (img->GetTextureX() + img->GetTextureW()) * itw,
                                         (img->GetTextureY() + img->GetTextureH()) * ith);
                 } else {
                     return true;
                 }
                 
                 shader = widget->getShader();
                 
                 g.SetShader(shader);
                 Sandbox::Transform2d mTr = Sandbox::Transform2d();
                 
                 
                 
                 int w = mCroppedParent->getWidth();
                 int h = mCroppedParent->getHeight();
                 
                 
                 mTr.translate(fill_texture_uv.left,fill_texture_uv.top);
                 mTr.scale(fill_texture_uv.width() / w,fill_texture_uv.height() / h);
                 
                 g.SetMask(MASK_MODE_ALPHA, fill_texture, mTr);
                 
             }
            return true;
        }
        void MaskText::EndPass(Graphics& g,const FontPass& pass) const {
            g.SetMask(MASK_MODE_NONE, TexturePtr(), Transform2d());
            g.SetShader(ShaderPtr());
            Base::EndPass(g, pass);
        }
        
    }
}
