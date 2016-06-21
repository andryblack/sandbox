#include "sb_mygui_text_skin.h"
#include "meta/sb_meta.h"
#include "MyGUI_RenderItem.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::AutoSizeText,MyGUI::EditText)


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
            
            
            const MyGUI::RenderTargetInfo& renderTargetInfo = _target->getInfo();
            
            // колличество отрисованных вершин
            size_t vertexCount = 0;
            
            // текущие цвета
            MyGUI::uint32 colour = mCurrentColourNative;
            
            const MyGUI::VectorLineInfo& textViewData = mTextView.getData();
            
            
            
            MyGUI::FloatRect vertexRect;
                        
            
            
            for (size_t pass=0;pass<mFont->getNumPasses();++pass) {
                std::map<std::string,MyGUI::Colour>::const_iterator clrit = mPassColour.find(mFont->getPassName(pass));
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
                            drawGlyph(renderTargetInfo, _target, vertexCount, vertexRect, info->uvRect, colour);
                        }
                        
                        left += fullAdvance * m_scale;
                    }
                    
                    top += mFontHeight * m_scale;
                }
            }
            
           
        }
    }
}