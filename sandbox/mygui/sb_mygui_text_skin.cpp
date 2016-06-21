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
            MyGUI::uint32 selectedColour =  colour | 0x00FFFFFF;
            
            const MyGUI::VectorLineInfo& textViewData = mTextView.getData();
            
            float top = (float)(-mViewOffset.top + mCoord.top);
            
            MyGUI::FloatRect vertexRect;
            
            
            size_t index = 0;
            
            for (MyGUI::VectorLineInfo::const_iterator line = textViewData.begin(); line != textViewData.end(); ++line)
            {
                float left = (float)(float(line->offset)*m_scale - mViewOffset.left + mCoord.left);
                
                for (MyGUI::VectorCharInfo::const_iterator sim = line->simbols.begin(); sim != line->simbols.end(); ++sim)
                {
                    if (sim->isColour())
                    {
                        colour = sim->getColour() | (colour & 0xFF000000);
                        selectedColour = colour | 0x00FFFFFF;
                        continue;
                    }
                    
                    float fullAdvance = sim->getBearingX() + sim->getAdvance();
                                        
                    _target->setTexture(sim->getTexture());
                    
                    // Render the glyph shadow, if any.
                    if (mShadow)
                    {
                        vertexRect.left = left + (sim->getBearingX() + 1.0f)*m_scale;
                        vertexRect.top = top + (sim->getBearingY() + 1.0f)*m_scale;
                        vertexRect.right = vertexRect.left + sim->getWidth() * m_scale;
                        vertexRect.bottom = vertexRect.top + sim->getHeight() * m_scale;
                        
                        drawGlyph(renderTargetInfo, _target, vertexCount, vertexRect, sim->getUVRect(), mShadowColourNative);
                    }
                    
                    // Render the glyph itself.
                    vertexRect.left = left + sim->getBearingX() * m_scale;
                    vertexRect.top = top + sim->getBearingY() * m_scale;
                    vertexRect.right = vertexRect.left + sim->getWidth() * m_scale;
                    vertexRect.bottom = vertexRect.top + sim->getHeight() * m_scale;
                    
                    drawGlyph(renderTargetInfo, _target, vertexCount, vertexRect, sim->getUVRect(), colour);
                    
                    left += fullAdvance * m_scale;
                    ++index;
                }
                
                top += mFontHeight * m_scale;
                ++index;
            }
            
           
        }
    }
}