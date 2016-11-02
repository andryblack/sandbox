/*
 * This source file is part of MyGUI. For the latest info, see http://mygui.info/
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "MyGUI_Precompiled.h"
#include "sb_mygui_text_view.h"
#include "MyGUI_FontData.h"

namespace Sandbox
{
    namespace mygui {


        template<typename T>
        static inline void setMin(T& _var, const T& _newValue)
        {
            if (_newValue < _var)
                _var = _newValue;
        }

        template<typename T>
        static inline void setMax(T& _var, const T& _newValue)
        {
            if (_var < _newValue)
                _var = _newValue;
        }

	
	TextView::TextView()
	{
        mTextData.Clear();
	}

        
	void TextView::update(const MyGUI::UString& _text, const Sandbox::FontPtr& _font , MyGUI::Align _align, int _maxWidth)
	{
		
        m_font = _font;
		mViewSize.clear();
        mTextData.Clear();
        
        FontAlign align = ALIGN_LEFT;
        if (_align.isHCenter())
            align = ALIGN_CENTER;
        if (_align.isRight())
            align = ALIGN_RIGHT;
        
        mTextData.Fill(_text.c_str(), _font, _maxWidth, align);

		mViewSize = MyGUI::IntSize(mTextData.size.w,mTextData.size.h);
	}
    
   

	size_t TextView::getCursorPosition(const MyGUI::IntPoint& _value)
	{
		size_t result = 0;
        if (!m_font)
            return 0;
      
        float descent = m_font->GetBaseline();

        for (TextData::LinesData::const_iterator line = mTextData.data.begin(); line != mTextData.data.end(); ++line)
		{
            const TextData::Line& l(*line);
			// это последняя строка
			bool lastline = !(line + 1 != mTextData.data.end());

			// наша строчка
			if ( ((l.offset.y + descent) > _value.top) || lastline)
			{
				float left = (float)line->offset.x;
				int count = 0;

				// ищем символ
                for (TextData::SymbolsData::const_iterator sim = line->data.begin(); sim != line->data.end(); ++sim)
				{
                    if (sim->type!=TextData::SYMBOL_TEXT)
						continue;

                    float fullAdvance = sim->data.symbol.offset;
					if (left + fullAdvance / 2.0f > _value.left)
					{
						break;
					}
					left += fullAdvance;
					count ++;
				}

				result += count;
				break;
			}

			if (!lastline)
			{
				result += line->count + 1;
			}
		}

		return result;
	}

	MyGUI::IntPoint TextView::getCursorPoint(size_t _position)
	{
		setMin(_position, mTextData.length);

		size_t position = 0;
		int top = 0;
		float left = 0.0f;
        for (TextData::LinesData::const_iterator line = mTextData.data.begin(); line != mTextData.data.end(); ++line)
		{
			left = (float)line->offset.x;
			if (position + line->count >= _position)
			{
                for (TextData::SymbolsData::const_iterator sim = line->data.begin(); sim != line->data.end(); ++sim)
				{
                    if (sim->type != TextData::SYMBOL_TEXT)
						continue;

					if (position == _position)
						break;

                	position ++;
                    left += sim->data.symbol.offset;
				}
                top = line->offset.y;
				break;
			}
			position += line->count + 1;
		}

		return MyGUI::IntPoint((int)left, top);
	}

	const MyGUI::IntSize& TextView::getViewSize() const
	{
		return mViewSize;
	}

	size_t TextView::getTextLength() const
	{
		return mTextData.length;
	}

	const TextData& TextView::getData() const
	{
		return mTextData;
	}
        
    }

} // namespace Sandbox
