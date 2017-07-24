/*
 * This source file is part of MyGUI. For the latest info, see http://mygui.info/
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef MYGUI_EDIT_TEXT_H_
#define MYGUI_EDIT_TEXT_H_

#include "MyGUI_Prerequest.h"
#include "MyGUI_XmlDocument.h"
#include "MyGUI_Types.h"
#include "MyGUI_ISubWidgetText.h"
#include "MyGUI_ResourceSkin.h"
#include "sb_mygui_text_view.h"
#include "MyGUI_VertexData.h"
#include "sb_font.h"

namespace MyGUI {
    class RenderItem;
}

namespace Sandbox
{
    namespace mygui {

        class EditTextStateInfo :
            public MyGUI::IStateInfo
        {
            MYGUI_RTTI_DERIVED( EditTextStateInfo )
            
        public:
            EditTextStateInfo() :
            mColour(MyGUI::Colour::White),
            mShift(false)
            {
            }
            
            virtual ~EditTextStateInfo() { }
            
            const MyGUI::Colour& getColour() const
            {
                return mColour;
            }
            
            bool getShift() const
            {
                return mShift;
            }
            
        private:
            virtual void deserialization(MyGUI::xml::ElementPtr _node, MyGUI::Version _version);
        private:
            MyGUI::Colour mColour;
            bool mShift;
        };

        class EditText;
        
        class TextDrawAttributes : public FontDrawAttributes {
        SB_META_OBJECT
        public:
            EditText* parent;
            virtual bool BeginPass(Graphics& g, const FontPass& pass) const;
            virtual void EndPass(Graphics& g, const FontPass& pass) const;
        };

	class EditText :
        public MyGUI::ISubWidgetText,
        public FontDrawAttributes
	{
		MYGUI_RTTI_DERIVED( EditText )

	public:
		EditText();
		virtual ~EditText();

		virtual void setVisible(bool _value);

		// обновляет все данные связанные с тектом
		virtual void updateRawData();

		// метод для отрисовки себя
        virtual void doRender(MyGUI::IRenderTarget* _target);

		void setCaption(const MyGUI::UString& _value);
		const MyGUI::UString& getCaption() const;

		void setTextColour(const MyGUI::Colour& _value);
		const MyGUI::Colour& getTextColour() const;

		void setAlpha(float _value);
		float getAlpha() const;

		virtual void setFontName(const std::string& _value);
        virtual const std::string& getFontName() const { return mFontName; }
        
		virtual void setFontHeight(int _value);
		virtual int getFontHeight() const;

		virtual void createDrawItem(MyGUI::ITexture* _texture, MyGUI::ILayerNode* _node);
		virtual void destroyDrawItem();

		virtual void setTextAlign(MyGUI::Align _value);
		virtual MyGUI::Align getTextAlign() const;

		virtual bool isVisibleCursor() const;
		virtual void setVisibleCursor(bool _value);

		virtual size_t getCursorPosition() const;
		virtual void setCursorPosition(size_t _index);

		virtual MyGUI::IntSize getTextSize() const;

		// устанавливает смещение текста в пикселях
		virtual void setViewOffset(const MyGUI::IntPoint& _point);
		virtual MyGUI::IntPoint getViewOffset() const;

		// возвращает положение курсора по произвольному положению
		virtual size_t getCursorPosition(const MyGUI::IntPoint& _point);

		// возвращает положение курсора в обсолютных координатах
		virtual MyGUI::IntCoord getCursorCoord(size_t _position);

		virtual void setShiftText(bool _shift);

        virtual bool getWordWrap() const;
		virtual void setWordWrap(bool _value);

		virtual void setStateData(MyGUI::IStateInfo* _data);

        virtual void setPassColour(const std::string& pass,const MyGUI::Colour& _value);
		
		/*internal:*/
		virtual void _updateView();
		virtual void _correctView();

		virtual void _setAlign(const MyGUI::IntSize& _oldsize);

        virtual bool fontHasSymbol(MyGUI::Char s) const;
        
        virtual bool BeginPass(Graphics& g,const FontPass& pass) const;
        virtual void EndPass(Graphics& g,const FontPass& pass) const;
	private:
		void _setTextColour(const MyGUI::Colour& _value);
		
    protected:
		


	protected:
		bool mEmptyView;
		MyGUI::IntCoord mCurrentCoord;

		MyGUI::UString mCaption;
		bool mTextOutDate;
		MyGUI::Align mTextAlign;

		MyGUI::Colour mColour;
        typedef sb::map<sb::string, MyGUI::Colour> PassColourMap;
        PassColourMap mPassColors;
        
		float mAlpha;
	
        std::string mFontName;
		FontPtr mFont;
		int mFontHeight;
        float GetFontScale() const;

		size_t mCursorPosition;
		bool mVisibleCursor;
		
		MyGUI::FloatPoint mViewOffset; // смещение текста

		MyGUI::ILayerNode* mNode;
		MyGUI::RenderItem* mRenderItem;
		size_t mCountVertex;
		bool mIsAddCursorWidth;

		bool mShiftText;
		bool mWordWrap;
		int mOldWidth;

		TextView mTextView;
        TextDrawAttributes  m_attributes;
	};
        
    }

} // namespace Sandbox

#endif // MYGUI_EDIT_TEXT_H_
