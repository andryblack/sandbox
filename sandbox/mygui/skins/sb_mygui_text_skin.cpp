#include "sb_mygui_text_skin.h"
#include "meta/sb_meta.h"
#include "MyGUI_RenderItem.h"
#include "../widgets/sb_mygui_mask_text.h"
#include "mygui/sb_mygui_render.h"
#include "sb_resources.h"
#include "sb_graphics.h"
#include "sb_mygui_skin.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::AutoSizeText,Sandbox::mygui::EditText)
SB_META_DECLARE_OBJECT(Sandbox::mygui::MaskText,Sandbox::mygui::AutoSizeText)


namespace Sandbox {
    
    namespace mygui {
        
        AutoSizeText::AutoSizeText() {
            mIsAddCursorWidth = false;
            m_scale = 1.0f;
        }
        
        AutoSizeText::~AutoSizeText() {
            
        }
        
        void AutoSizeText::setViewOffset(const MyGUI::IntPoint& _point) {
            
        }
        
        void AutoSizeText::updateRawData() {
            if (nullptr == mFont)
                return;
            // сбрасывам флаги
            mTextOutDate = false;
            
            int width = -1;
            if (mWordWrap)
            {
                width = mCoord.width / m_scale;
                // обрезать слова нужно по шарине, которую мы реально используем
                if (mIsAddCursorWidth)
                    width -= 2;
            }
            
            mTextView.update(mCaption, mFont, mFontHeight, mTextAlign, width);
        }
        
        void AutoSizeText::doRender(MyGUI::IRenderTarget* _target) {
                        
            if (nullptr == mFont)
                return;
            if (!mVisible || mEmptyView)
                return;
            
            if (mTextOutDate) {
                m_scale = 1.0f;
                updateRawData();
            }
            
            MyGUI::IntSize size = mTextView.getViewSize();
            
            float hscale = 1.0f;
            if (size.width > mCoord.width) {
                hscale = float(mCoord.width) / float(size.width);
                if (hscale < m_scale) {
                    m_scale = hscale;
                }
            } else if (!mWordWrap) {
                m_scale = 1.0f;
            }
            
            if (mWordWrap) {
                while ((size.height*m_scale) > mCoord.height) {
                    m_scale -= 0.125f/4;
                    if (m_scale < 0.125f) {
                        m_scale = 0.125f;
                    }
                    updateRawData();
                    size = mTextView.getViewSize();
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
            
            
            
            // текущие цвета
            MyGUI::uint32 colour = mCurrentColourNative;
            
            const TextData& textViewData = mTextView.getData();
            
            Graphics& g = *(static_cast<RenderManager*>(MyGUI::RenderManager::getInstancePtr())->graphics());
            Color c = g.GetColor();
            g.SetColor(c * Color(colour));
            Transform2d tr = g.GetTransform();
            g.SetTransform(tr.translated(mCroppedParent->getAbsoluteLeft(), mCroppedParent->getAbsoluteTop())
                           .scale(m_scale));
            MyGUI::FloatRect vertexRect;
            
            float top = (float)(-mViewOffset.top + mCoord.top);
            float left = (float)( - mViewOffset.left + mCoord.left);
            
            mFont->Draw(g, &m_attributes, Vector2f(left,top), textViewData);
            g.SetColor(c);
            g.SetTransform(tr);
        }
        
        
        MaskText::MaskText() {
            
        }
        
        MaskText::~MaskText() {
            
        }
        
        bool MaskText::BeginPass(Graphics& g,const FontPass& pass) {
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
        void MaskText::EndPass(Graphics& g,const FontPass& pass) {
            g.SetMask(MASK_MODE_NONE, TexturePtr(), Transform2d());
            g.SetShader(ShaderPtr());
            Base::EndPass(g, pass);
        }
        
    }
}
