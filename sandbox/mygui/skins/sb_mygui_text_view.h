/*
 * This source file is part of MyGUI. For the latest info, see http://mygui.info/
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef MYGUI_TEXT_VIEW_H_
#define MYGUI_TEXT_VIEW_H_

#include "MyGUI_Prerequest.h"
#include "MyGUI_TextureUtility.h"
#include "sb_font.h"
#include "sb_text_data.h"

namespace Sandbox
{
    
    namespace mygui {

	class TextView
	{
	public:
		TextView();

        void update(const MyGUI::UString& _text,
                    const Sandbox::FontPtr& _font,
                    MyGUI::Align _align,
                    int _maxWidth = -1);

		size_t getCursorPosition(const MyGUI::IntPoint& _value);

		MyGUI::IntPoint getCursorPoint(size_t _position);

		const MyGUI::IntSize& getViewSize() const;

		size_t getTextLength() const;

		const TextData& getData() const;

	private:
		MyGUI::IntSize mViewSize;
		TextData mTextData;
        FontPtr   m_font;
    };

    }
} // namespace Sandbox

#endif // MYGUI_TEXT_VIEW_H_
