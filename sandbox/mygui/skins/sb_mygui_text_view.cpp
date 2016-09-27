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

	namespace
	{

		template<typename T>
		void setMin(T& _var, const T& _newValue)
		{
			if (_newValue < _var)
				_var = _newValue;
		}

		template<typename T>
		void setMax(T& _var, const T& _newValue)
		{
			if (_var < _newValue)
				_var = _newValue;
		}

	}

	class RollBackPoint
	{
	public:
		RollBackPoint() :
			position(0),
			count(0),
			width(0),
			rollback(false)
		{
		}

		void set(size_t _position, MyGUI::UString::iterator& _space_point, size_t _count, float _width)
		{
			position = _position;
			space_point = _space_point;
			count = _count;
			width = _width;
			rollback = true;
		}

		void clear()
		{
			rollback = false;
		}

		bool empty() const
		{
			return !rollback;
		}

		float getWidth() const
		{
			MYGUI_DEBUG_ASSERT(rollback, "rollback point not valid");
			return width;
		}

		size_t getCount() const
		{
			MYGUI_DEBUG_ASSERT(rollback, "rollback point not valid");
			return count;
		}

		size_t getPosition() const
		{
			MYGUI_DEBUG_ASSERT(rollback, "rollback point not valid");
			return position;
		}

		MyGUI::UString::iterator getTextIter() const
		{
			MYGUI_DEBUG_ASSERT(rollback, "rollback point not valid");
			return space_point;
		}

	private:
		size_t position;
		MyGUI::UString::iterator space_point;
		size_t count;
		float width;
		bool rollback;
	};

	TextView::TextView() :
		mLength(0),
		mFontHeight(0)
	{
	}

        
	void TextView::update(const MyGUI::UString& _text, const Sandbox::FontPtr& _font , int _height, MyGUI::Align _align, int _maxWidth)
	{
		mFontHeight = _height;

		// массив для быстрой конвертации цветов
		static const char convert_colour[64] =
		{
			0,  1,  2,  3,  4,  5,  6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
			0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};

		mViewSize.clear();

		RollBackPoint roll_back;
		MyGUI::IntSize result;
		float width = 0.0f;
		size_t count = 0;
		mLength = 0;
		mTextData.Clear();
        
		TextData text_data;
		 
        TextData::Line line_info;
        line_info.offset.y = _font->GetHeight() - _font->GetBaseline();
        
		MyGUI::UString::iterator end = _text.end();
		MyGUI::UString::iterator index = _text.begin();

		result.height += _height;

		for (; index != end; ++index)
		{
            MyGUI::Char character = *index;

			// новая строка
			if (character == MyGUI::FontCodeType::CR
				|| character == MyGUI::FontCodeType::NEL
				|| character == MyGUI::FontCodeType::LF)
			{
				if (character == MyGUI::FontCodeType::CR)
				{
					MyGUI::UString::iterator peeki = index;
					++peeki;
					if ((peeki != end) && (*peeki == MyGUI::FontCodeType::LF))
						index = peeki; // skip both as one newline
				}

				line_info.width = (int)ceil(width);
				line_info.count = count;
				mLength += line_info.count + 1;

				result.height += _height;
				
				width = 0;
				count = 0;

				mTextData.data.push_back(line_info);
				line_info.Clear();
                line_info.offset.y += _height;

				// отменяем откат
				roll_back.clear();

              	continue;
			}
			// тег
			else if (character == L'#')
			{
				// берем следующий символ
				++ index;
				if (index == end)
				{
                    break;
				}

				character = *index;
				// если два подряд, то рисуем один шарп, если нет то меняем цвет
				if (character != L'#')
				{
					// парсим первый символ
					MyGUI::uint32 colour = convert_colour[(character - 48) & 0x3F];

					// и еще пять символов после шарпа
					for (char i = 0; i < 5; i++)
					{
						++ index;
						if (index == end)
						{
                            break;
						}
						colour <<= 4;
						colour += convert_colour[ ((*index) - 48) & 0x3F ];
					}
                    colour = ((colour & 0x00ff0000) >> 16) |
                             ((colour & 0x000000ff) << 16) |
                             ((colour & 0xff00ff00)) | 0xff000000;
                    TextData::SymbolData symbol;
                    symbol.type = TextData::SYMBOL_SET_COLOR;
                    symbol.data.color = colour;
                    line_info.data.push_back(symbol);
                    if (index==end)
                        break;
					continue;
				}
			}

            const FontData::Glypth* info = _font->GetGlyph(character);

			if (info == nullptr)
				continue;

			if (MyGUI::FontCodeType::Space == character)
			{
				roll_back.set(line_info.data.size(), index, count, width);
			}
			else if (MyGUI::FontCodeType::Tab == character)
			{
				roll_back.set(line_info.data.size(), index, count, width);
			}

            float char_advance = info->asc;
			
			float char_fullAdvance = char_advance;

			// перенос слов
			if (_maxWidth != -1
				&& (width + char_fullAdvance) > _maxWidth
				&& !roll_back.empty())
			{
				// откатываем до последнего пробела
				width = roll_back.getWidth();
				count = roll_back.getCount();
				index = roll_back.getTextIter();
				line_info.data.erase(line_info.data.begin() + roll_back.getPosition(), line_info.data.end());

				// запоминаем место отката, как полную строку
				line_info.width = (int)ceil(width);
				line_info.count = count;
				mLength += line_info.count + 1;

				result.height += _height;
				
				width = 0;
				count = 0;

				mTextData.data.push_back(line_info);
				line_info.Clear();
                line_info.offset.y += _height;

				// отменяем откат
				roll_back.clear();

				continue;
			}
            TextData::SymbolData symbol;
            symbol.type = TextData::SYMBOL_TEXT;
            symbol.data.symbol.code = character;
            symbol.data.symbol.offset = char_fullAdvance;
            line_info.data.push_back(symbol);
			width += char_fullAdvance;
			count ++;
		}

		line_info.width = (int)ceil(width);
		line_info.count = count;
		mLength += line_info.count;

		mTextData.data.push_back(line_info);
        result.width = 0.0f;
        float offset = 0.0f;
        for (TextData::LinesData::iterator line = mTextData.data.begin(); line != mTextData.data.end(); ++line)
        {
            line->offset.x = 0;
            width = line->width;
            if (!line->data.empty()) {
                for (TextData::SymbolsData::const_iterator simbol = line->data.begin();simbol!=line->data.end();++simbol) {
                    if (simbol->type == TextData::SYMBOL_TEXT) {
                        const FontData::Glypth* info = _font->GetGlyph(simbol->data.symbol.code);
                        if (info) {
                            if (info->bearingX() < 0) {
                                float firstOffset = - info->bearingX();
                                setMax(offset, firstOffset);
                                width += firstOffset;
                            }
                        }
                        break;
                    }
                }
                for (TextData::SymbolsData::const_reverse_iterator simbol = line->data.rbegin();simbol!=line->data.rend();++simbol) {
                    if (simbol->type == TextData::SYMBOL_TEXT) {
                        const FontData::Glypth* info = _font->GetGlyph(simbol->data.symbol.code);
                        if (info) {
                            float rightOffset = ((info->bearingX() + info->width()) - info->asc);
                            if (rightOffset > 0) {
                                width += rightOffset;
                            }
                        }
                        break;
                    }
                }
            }
            line->width = (int)ceil(width);
            setMax(result.width, int(line->width));
        }
        
        if (!mTextData.data.empty()) {
            int addHeight = 0;
            for (TextData::SymbolsData::const_reverse_iterator simbol = mTextData.data.back().data.rbegin();simbol!=mTextData.data.back().data.rend();++simbol) {
                if (simbol->type == TextData::SYMBOL_TEXT) {
                    const FontData::Glypth* info = _font->GetGlyph(simbol->data.symbol.code);
                    if (info) {
                        int bottomOffset = int(ceil(((info->bearingY() + info->height()) - _height)));
                        if (bottomOffset > 0) {
                            setMax(addHeight, bottomOffset);
                        }
                    }
                    break;
                }
            }
            
            result.height += addHeight;
        }
        
		
		// теперь выравниванием строки
		for (TextData::LinesData::iterator line = mTextData.data.begin(); line != mTextData.data.end(); ++line)
		{
            line->offset.x = offset;
			if (_align.isRight())
				line->offset.x += result.width - line->width;
			else if (_align.isHCenter())
				line->offset.x += (result.width - line->width) / 2;
		}
        

		mViewSize = result;
	}
    
   

	size_t TextView::getCursorPosition(const MyGUI::IntPoint& _value)
	{
		const int height = mFontHeight;
		size_t result = 0;
		int top = 0;

        for (TextData::LinesData::const_iterator line = mTextData.data.begin(); line != mTextData.data.end(); ++line)
		{
			// это последняя строка
			bool lastline = !(line + 1 != mTextData.data.end());

			// наша строчка
			if (top + height > _value.top || lastline)
			{
				top += height;
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
				top += height;
				result += line->count + 1;
			}
		}

		return result;
	}

	MyGUI::IntPoint TextView::getCursorPoint(size_t _position)
	{
		setMin(_position, mLength);

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
		return mLength;
	}

	const TextData& TextView::getData() const
	{
		return mTextData;
	}
        
    }

} // namespace Sandbox
