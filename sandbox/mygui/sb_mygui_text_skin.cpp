#include "sb_mygui_text_skin.h"
#include "meta/sb_meta.h"
#include "MyGUI_RenderItem.h"
#include "widgets/sb_mygui_mask_text.h"
#include "mygui/sb_mygui_render.h"
#include "sb_resources.h"
#include "sb_graphics.h"
#include "sb_mygui_skin.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::AutoSizeText,MyGUI::EditText)
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
            
            if (nullptr == mFont || !mVisible || mEmptyView)
                return;
            
            if (mRenderItem->getCurrentUpdate() || mTextOutDate)
                updateRawData();
            
          
            
            
            
            
                        
            
            
            for (size_t pass=0;pass<mFont->getNumPasses();++pass) {
                renderPass(_target, pass);
            }
            
           
        }
        
        
        void AutoSizeText::renderPass(MyGUI::IRenderTarget* _target,size_t pass) {
            MyGUI::uint32 colour = mCurrentColourNative;
            const std::string& passName = mFont->getPassName(pass);
            
            // колличество отрисованных вершин
            size_t vertexCount = 0;
            
            MyGUI::FloatRect vertexRect;
            
            const MyGUI::VectorLineInfo& textViewData = mTextView.getData();
            
            std::map<std::string,MyGUI::Colour>::const_iterator clrit = mPassColour.find(passName);
            bool fixed_color = clrit != mPassColour.end();
            if (fixed_color) {
                MyGUI::Colour pass_colour = clrit->second;
                pass_colour.alpha *= mAlpha;
                colour = MyGUI::texture_utility::toColourARGB(pass_colour);
            } else {
                colour = mCurrentColourNative;
            }
            MyGUI::FloatSize offset = mFont->getOffset(pass);
            float  top = (float)(-mViewOffset.top + mCoord.top) + offset.height;
            
            for (MyGUI::VectorLineInfoLines::const_iterator line = textViewData.lines.begin(); line != textViewData.lines.end(); ++line)
            {
                float left = (float)(float(line->offset)*m_scale - mViewOffset.left + mCoord.left) + offset.width * m_scale;
                
                for (MyGUI::VectorCharInfo::const_iterator sim = line->simbols.begin(); sim != line->simbols.end(); ++sim)
                {
                    if (sim->isColour())
                    {
                        if (!fixed_color) {
                            colour = sim->getColour() | (colour & 0xFF000000);
                        }
                        continue;
                    }
                    
                    float fullAdvance = sim->getAdvance();
                    const MyGUI::GlyphInfo* info = mFont->getGlyphInfo(pass, sim->getChar());
                    if (info) {
                        _target->setTexture(info->texture);
                        vertexRect.left = left + (info->bearingX * textViewData.scale )*m_scale;
                        vertexRect.top = top + (info->bearingY * textViewData.scale )*m_scale;
                        vertexRect.right = vertexRect.left + info->width * textViewData.scale * m_scale;
                        vertexRect.bottom = vertexRect.top + info->height * textViewData.scale * m_scale;
                        drawGlyph(_target, vertexCount, vertexRect, info->uvRect, colour);
                    }
                    
                    left += fullAdvance * m_scale;
                }
                
                top += mFontHeight * m_scale;
            }

        }
        
        MaskText::MaskText() {
            
        }
        
        MaskText::~MaskText() {
            
        }
        
        void MaskText::renderPass(MyGUI::IRenderTarget* _target,size_t pass) {
            if (mRenderItem) {
                const std::string& passName = mFont->getPassName(pass);
                
                RenderTargetImpl* target = static_cast<RenderTargetImpl*>(_target);
                MyGUI::Widget* widget_p = static_cast<MyGUI::Widget*>(mCroppedParent);
                MaskTextWidget* widget = widget_p->castType<MaskTextWidget>(false);
                if (!widget) {
                    Base::renderPass(_target, pass);
                    return;
                }
                ShaderPtr shader;
                TexturePtr fill_texture;
                MyGUI::FloatRect fill_texture_uv;
                
                ImagePtr    img = widget->getPassImage(passName);
                if (img && img->GetTexture()) {
                    fill_texture = img->GetTexture();
                    float itw = 1.0f / img->GetTexture()->GetWidth();
                    float ith = 1.0f / img->GetTexture()->GetHeight();
                    
                    fill_texture_uv.set( img->GetTextureX() * itw,
                                        img->GetTextureY() * ith,
                                        (img->GetTextureX() + img->GetTextureW()) * itw,
                                        (img->GetTextureY() + img->GetTextureH()) * ith);
                } else {
                    Base::renderPass(_target, pass);
                    return;
                }
                shader = widget->getShader();
                
                Graphics* g = target->graphics();
                
                if (g) {
                    g->SetShader(shader);
                    Sandbox::Transform2d mTr = Sandbox::Transform2d();
                    
                    
                    
                    int x = mCroppedParent->getAbsoluteLeft();
                    int y = mCroppedParent->getAbsoluteTop();
                    int w = mCroppedParent->getWidth();
                    int h = mCroppedParent->getHeight();
                    
                    
                    mTr.translate(fill_texture_uv.left,fill_texture_uv.top);
                    mTr.scale(fill_texture_uv.width() / w,fill_texture_uv.height() / h);
                    mTr.translate(-x,-y);
                    
                    g->SetMask(MASK_MODE_ALPHA, fill_texture, mTr);
                    Base::renderPass(_target,pass);
                    g->SetMask(MASK_MODE_NONE, TexturePtr(), Transform2d());
                    g->SetShader(ShaderPtr());
                    
                    //
                } else {
                    sb_assert(false);
                }

            }
        }
        
    }
}