/*
 * This source file is part of MyGUI. For the latest info, see http://mygui.info/
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "MyGUI_Precompiled.h"
#include "sb_mygui_edit_text.h"
#include "MyGUI_RenderItem.h"
#include "MyGUI_RenderManager.h"
#include "MyGUI_LanguageManager.h"
#include "MyGUI_TextIterator.h"
#include "MyGUI_IRenderTarget.h"
#include "MyGUI_FontData.h"
#include "MyGUI_CommonStateInfo.h"

#include "meta/sb_meta.h"
#include "../sb_mygui_render.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::EditText, MyGUI::ISubWidgetText)
SB_META_DECLARE_OBJECT(Sandbox::mygui::EditTextStateInfo, MyGUI::IStateInfo)
SB_META_DECLARE_OBJECT(Sandbox::mygui::TextDrawAttributes, Sandbox::FontDrawAttributes)

namespace Sandbox
{
    namespace mygui {
        
        
        void EditTextStateInfo::deserialization(MyGUI::xml::ElementPtr _node, MyGUI::Version _version) {
            mShift = MyGUI::utility::parseBool(_node->findAttribute("shift"));
            
            std::string colour = _node->findAttribute("colour");
            if (_version >= MyGUI::Version(1, 1))
            {
                colour = MyGUI::LanguageManager::getInstance().replaceTags(colour);
            }
            
            mColour = MyGUI::Colour::parse(colour);
        }
        
        bool TextDrawAttributes::BeginPass(Sandbox::Graphics &g, const Sandbox::FontPass &pass) {
            return parent->BeginPass(g, pass);
        }
        void TextDrawAttributes::EndPass(Sandbox::Graphics &g, const Sandbox::FontPass &pass) {
            parent->EndPass(g, pass);
        }

	const size_t VERTEX_IN_QUAD = 4;
	const size_t SIMPLETEXT_COUNT_VERTEX = 32 * VERTEX_IN_QUAD;

	EditText::EditText() :
		ISubWidgetText(),
		mEmptyView(false),
		mCurrentColourNative(0x00FFFFFF),
		mInverseColourNative(0x00000000),
		mCurrentAlphaNative(0xFF000000),
		mTextOutDate(false),
		mTextAlign(MyGUI::Align::Default),
		mColour(MyGUI::Colour::White),
		mAlpha(MyGUI::ALPHA_MAX),
		mFontHeight(0),
		mCursorPosition(0),
		mVisibleCursor(false),
		mNode(nullptr),
		mRenderItem(nullptr),
		mCountVertex(SIMPLETEXT_COUNT_VERTEX),
		mIsAddCursorWidth(true),
		mShiftText(false),
		mWordWrap(false),
		mManualColour(false),
		mOldWidth(0)
	{
      
        mCurrentColourNative = MyGUI::texture_utility::toColourARGB(mColour);
	
		mCurrentColourNative = (mCurrentColourNative & 0x00FFFFFF) | (mCurrentAlphaNative & 0xFF000000);
		mInverseColourNative = mCurrentColourNative ^ 0x00FFFFFF;
        
        m_attributes.parent = this;
	}

	EditText::~EditText()
	{
	}

	void EditText::setVisible(bool _visible)
	{
		if (mVisible == _visible)
			return;
		mVisible = _visible;

		if (nullptr != mNode)
			mNode->outOfDate(mRenderItem);
	}

	void EditText::_correctView()
	{
		if (nullptr != mNode)
			mNode->outOfDate(mRenderItem);
	}

	void EditText::_setAlign(const MyGUI::IntSize& _oldsize)
	{
		if (mWordWrap)
		{
			// передается старая координата всегда
			int width = mCroppedParent->getWidth();
			if (mOldWidth != width)
			{
				mOldWidth = width;
				mTextOutDate = true;
			}
		}

		// необходимо разобраться
		bool need_update = true;//_update;

		// первоначальное выравнивание
		if (mAlign.isHStretch())
		{
			// растягиваем
			mCoord.width = mCoord.width + (mCroppedParent->getWidth() - _oldsize.width);
			need_update = true;
			mIsMargin = true; // при изменении размеров все пересчитывать
		}
		else if (mAlign.isRight())
		{
			// двигаем по правому краю
			mCoord.left = mCoord.left + (mCroppedParent->getWidth() - _oldsize.width);
			need_update = true;
		}
		else if (mAlign.isHCenter())
		{
			// выравнивание по горизонтали без растяжения
			mCoord.left = (mCroppedParent->getWidth() - mCoord.width) / 2;
			need_update = true;
		}

		if (mAlign.isVStretch())
		{
			// растягиваем
			mCoord.height = mCoord.height + (mCroppedParent->getHeight() - _oldsize.height);
			need_update = true;
			mIsMargin = true; // при изменении размеров все пересчитывать
		}
		else if (mAlign.isBottom())
		{
			// двигаем по нижнему краю
			mCoord.top = mCoord.top + (mCroppedParent->getHeight() - _oldsize.height);
			need_update = true;
		}
		else if (mAlign.isVCenter())
		{
			// выравнивание по вертикали без растяжения
			mCoord.top = (mCroppedParent->getHeight() - mCoord.height) / 2;
			need_update = true;
		}

		if (need_update)
		{
			mCurrentCoord = mCoord;
			_updateView();
		}
	}

	void EditText::_updateView()
	{
		bool margin = _checkMargin();

		mEmptyView = ((0 >= _getViewWidth()) || (0 >= _getViewHeight()));

		mCurrentCoord.left = mCoord.left + mMargin.left;
		mCurrentCoord.top = mCoord.top + mMargin.top;

		// вьюпорт стал битым
		if (margin)
		{
			// проверка на полный выход за границу
			if (_checkOutside())
			{
				// запоминаем текущее состояние
				mIsMargin = margin;

				// обновить перед выходом
				if (nullptr != mNode)
					mNode->outOfDate(mRenderItem);
				return;
			}
		}

		// мы обрезаны или были обрезаны
		if (mIsMargin || margin)
		{
			mCurrentCoord.width = _getViewWidth();
			mCurrentCoord.height = _getViewHeight();
		}

		// запоминаем текущее состояние
		mIsMargin = margin;

		if (nullptr != mNode)
			mNode->outOfDate(mRenderItem);
	}

	void EditText::setCaption(const MyGUI::UString& _value)
	{
		mCaption = _value;
		mTextOutDate = true;

        if (mFont) {
            mFont->AllocateSymbols(_value.c_str());
        }

		if (nullptr != mNode)
			mNode->outOfDate(mRenderItem);
	}


	const MyGUI::UString& EditText::getCaption() const
	{
		return mCaption;
	}

	void EditText::setTextColour(const MyGUI::Colour& _value)
	{
		mManualColour = true;
		_setTextColour(_value);
	}

	void EditText::_setTextColour(const MyGUI::Colour& _value)
	{
		if (mColour == _value)
			return;

		mColour = _value;
		mCurrentColourNative = MyGUI::texture_utility::toColourARGB(mColour);

		mCurrentColourNative = (mCurrentColourNative & 0x00FFFFFF) | (mCurrentAlphaNative & 0xFF000000);
		mInverseColourNative = mCurrentColourNative ^ 0x00FFFFFF;

		if (nullptr != mNode)
			mNode->outOfDate(mRenderItem);
	}

	const MyGUI::Colour& EditText::getTextColour() const
	{
		return mColour;
	}

	void EditText::setAlpha(float _value)
	{
		if (mAlpha == _value)
			return;
		mAlpha = _value;

		mCurrentAlphaNative = ((MyGUI::uint8)(mAlpha * 255) << 24);
		mCurrentColourNative = (mCurrentColourNative & 0x00FFFFFF) | (mCurrentAlphaNative & 0xFF000000);
		mInverseColourNative = mCurrentColourNative ^ 0x00FFFFFF;

		if (nullptr != mNode)
			mNode->outOfDate(mRenderItem);
	}

	float EditText::getAlpha() const
	{
		return mAlpha;
	}

	void EditText::setFontName(const std::string& _value)
	{
        if (mFontName == _value)
            return;
        mFontName = _value;
        mFont = static_cast<RenderManager*>(MyGUI::RenderManager::getInstancePtr())->getFont(mFontName);
        
        if (!mFont) {
            return;
        }
        
        mFont->AllocateSymbols(mCaption.c_str());

		mTextOutDate = true;

		// если мы были приаттаченны, то удаляем себя
		if (nullptr != mRenderItem)
		{
			mRenderItem->removeDrawItem(this);
			mRenderItem = nullptr;
		}

		// если есть текстура, то приаттачиваемся
		if (nullptr != mFont && nullptr != mNode)
		{
			mRenderItem = mNode->addToRenderItem(mFont.get(), false, false);
			mRenderItem->addDrawItem(this);
		}

		if (nullptr != mNode)
			mNode->outOfDate(mRenderItem);
	}


	void EditText::setFontHeight(int _value)
	{
		mFontHeight = _value;
		mTextOutDate = true;

		if (nullptr != mNode)
			mNode->outOfDate(mRenderItem);
	}

	int EditText::getFontHeight() const
	{
		return mFontHeight;
	}

	void EditText::createDrawItem(MyGUI::ITexture* _texture, MyGUI::ILayerNode* _node)
	{
		mNode = _node;
		// если уже есть текстура, то атачимся, актуально для смены леера
		if (nullptr != mFont)
		{
			MYGUI_ASSERT(!mRenderItem, "mRenderItem must be nullptr");

			mRenderItem = mNode->addToRenderItem(mFont.get(), false, false);
			mRenderItem->addDrawItem(this);
		}
	}

	void EditText::destroyDrawItem()
	{
		if (nullptr != mRenderItem)
		{
			mRenderItem->removeDrawItem(this);
			mRenderItem = nullptr;
		}
		mNode = nullptr;
	}

	bool EditText::isVisibleCursor() const
	{
		return mVisibleCursor;
	}

	void EditText::setVisibleCursor(bool _value)
	{
		if (mVisibleCursor == _value)
			return;
		mVisibleCursor = _value;

		if (nullptr != mNode)
			mNode->outOfDate(mRenderItem);
	}

	size_t EditText::getCursorPosition() const
	{
		return mCursorPosition;
	}

	void EditText::setCursorPosition(size_t _index)
	{
		if (mCursorPosition == _index)
			return;
		mCursorPosition = _index;

		if (nullptr != mNode)
			mNode->outOfDate(mRenderItem);
	}

	void EditText::setTextAlign(MyGUI::Align _value)
	{
		mTextAlign = _value;

		if (nullptr != mNode)
			mNode->outOfDate(mRenderItem);
	}

	MyGUI::Align EditText::getTextAlign() const
	{
		return mTextAlign;
	}

	MyGUI::IntSize EditText::getTextSize() const
	{
		// если нуно обновить, или изменились пропорции экрана
		if (mTextOutDate)
			const_cast<EditText*>(this)->updateRawData();

		MyGUI::IntSize size = mTextView.getViewSize();
        
        float scale = GetFontScale();
        size.width *= scale;
        size.height *= scale;
        
		// плюс размер курсора
		if (mIsAddCursorWidth)
			size.width += 2;

		return size;
	}
        
    float EditText::GetFontScale() const {
        if (!mFont)
            return 1.0f;
        if (mFontHeight==0)
            return 1.0f;
        return mFontHeight / mFont->GetHeight();
    }

	void EditText::setViewOffset(const MyGUI::IntPoint& _point)
	{
		mViewOffset = _point;

		if (nullptr != mNode)
			mNode->outOfDate(mRenderItem);
	}

	MyGUI::IntPoint EditText::getViewOffset() const
	{
		return mViewOffset;
	}

	size_t EditText::getCursorPosition(const MyGUI::IntPoint& _point)
	{
		if (nullptr == mFont)
			return 0;

		if (mTextOutDate)
			updateRawData();

		MyGUI::IntPoint point = _point;
		point -= mCroppedParent->getAbsolutePosition();
		point += mViewOffset;
		point -= mCoord.point();
        
        float scale = GetFontScale();
        point.left = point.left / scale;
        point.top = point.top / scale;

		return mTextView.getCursorPosition(point);
	}

	MyGUI::IntCoord EditText::getCursorCoord(size_t _position)
	{
		if (nullptr == mFont)
			return MyGUI::IntCoord();

		if (mTextOutDate)
			updateRawData();

		MyGUI::IntPoint point = mTextView.getCursorPoint(_position);
        
        float scale = GetFontScale();
        point.left *= scale;
        point.top *= scale;
        
		point += mCroppedParent->getAbsolutePosition();
		point -= mViewOffset;
		point += mCoord.point();

		return MyGUI::IntCoord(point.left, point.top, 2, mFontHeight);
	}

	void EditText::setShiftText(bool _value)
	{
		if (mShiftText == _value)
			return;
		mShiftText = _value;

		if (nullptr != mNode)
			mNode->outOfDate(mRenderItem);
	}
    
    bool EditText::getWordWrap() const
    {
        return mWordWrap;
    }

	void EditText::setWordWrap(bool _value)
	{
		mWordWrap = _value;
		mTextOutDate = true;

		if (nullptr != mNode)
			mNode->outOfDate(mRenderItem);
	}
        
    bool EditText::fontHasSymbol(MyGUI::Char s) const {
        if (!mFont)
            return true;
        return mFont->HasGlyph(s);
    }

	void EditText::updateRawData()
	{
		if (nullptr == mFont)
			return;
		// сбрасывам флаги
		mTextOutDate = false;

		int width = -1;
		if (mWordWrap)
		{
			width = mCoord.width;
			// обрезать слова нужно по шарине, которую мы реально используем
			if (mIsAddCursorWidth)
				width -= 2;
		}

		mTextView.update(mCaption, mFont, mTextAlign, width);
	}

	void EditText::setStateData(MyGUI::IStateInfo* _data)
	{
		EditTextStateInfo* data = _data->castType<EditTextStateInfo>();
		if (!mManualColour && data->getColour() != MyGUI::Colour::Zero)
			_setTextColour(data->getColour());
		setShiftText(data->getShift());
	}

	void EditText::doRender(MyGUI::IRenderTarget* _target)
	{
		if (nullptr == mFont || !mVisible || mEmptyView)
			return;

		if (mRenderItem->getCurrentUpdate() || mTextOutDate)
			updateRawData();

        const TextData& textViewData = mTextView.getData();

        Graphics& g = *(static_cast<RenderManager*>(MyGUI::RenderManager::getInstancePtr())->graphics());

        Color c = g.GetColor();
        g.SetColor(c * Color(mCurrentColourNative));
        
        float scale = GetFontScale();
		 
        float top = (-mViewOffset.top + mCoord.top) + mCroppedParent->getAbsoluteTop();
        float left = ( - mViewOffset.left + mCoord.left) + mCroppedParent->getAbsoluteLeft();
        
        Transform2d tr = g.GetTransform();
        g.SetTransform(tr.translated(left, top).scale(scale));
        
        mFont->Draw(g, &m_attributes, textViewData);
        
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

    void EditText::setPassColour(const std::string& pass,const MyGUI::Colour& _value)
	{
        mPassColors[pass] = _value;
       
		if (nullptr != mNode)
			mNode->outOfDate(mRenderItem);
	}
        
    bool EditText::BeginPass(Graphics& g,const FontPass& pass) {
        sb::map<sb::string, MyGUI::Colour>::const_iterator it = mPassColors.find(pass.GetName());
        if (it!=mPassColors.end()) {
            g.SetColor(g.GetColor()*Color(it->second.red,it->second.green,it->second.blue,it->second.alpha));
        }
        return true;
    }
    void EditText::EndPass(Graphics& g,const FontPass& pass) {
        
    }

    }
} // namespace Sandbox
