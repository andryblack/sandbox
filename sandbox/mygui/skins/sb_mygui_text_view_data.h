/*
 * This source file is part of MyGUI. For the latest info, see http://mygui.info/
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef MYGUI_TEXT_VIEW_DATA_H_
#define MYGUI_TEXT_VIEW_DATA_H_

#include "MyGUI_Prerequest.h"

namespace Sandbox
{
    namespace mygui {
        
    
    class ITexture;

	class CharInfo
	{
	public:
		CharInfo() :
            mIsColour(false)
		{
            mSymbol.mChar = 0;
            mSymbol.mAdvance = 0;
		}

        CharInfo( MyGUI::Char ch, float advance ) : mIsColour(false)
		{
            mSymbol.mChar = ch;
            mSymbol.mAdvance = advance;
		}

		CharInfo(MyGUI::uint32 _colour) :
			mIsColour(true),
			mColour(_colour)
		{ }

		bool isColour() const
		{
			return mIsColour;
		}

		float getAdvance() const
		{
            return mSymbol.mAdvance;
		}
        
        MyGUI::Char getChar() const {
            return mSymbol.mChar;
        }


		MyGUI::uint32 getColour() const
		{
			return mColour;
		}

	private:

		bool mIsColour;
        
		
		struct Symbol
		{
			MyGUI::Char mChar;
            float mAdvance;
		};

		union
		{
			Symbol mSymbol;
			MyGUI::uint32 mColour;
		};

	};

	typedef std::vector<CharInfo> VectorCharInfo;

	struct LineInfo
	{
		LineInfo() :
			width(0),
			offset(0),
			count(0)
		{
		}

		void clear()
		{
			width = 0;
			count = 0;
			simbols.clear();
			offset = 0;
		}

		int width;
		int offset;
		size_t count;
		VectorCharInfo simbols;
	};

    typedef std::vector<LineInfo> VectorLineInfoLines;
    struct VectorLineInfo {
        VectorLineInfoLines lines;
        float scale;
        void clear() {
            scale = 1.0f;
            lines.clear();
        }
    };
	

    }
} // namespace Sandbox

#endif // MYGUI_TEXT_VIEW_DATA_H_
