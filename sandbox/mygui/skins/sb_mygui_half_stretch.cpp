#include "sb_mygui_half_stretch.h"
#include "meta/sb_meta.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::SubSkinHalfHStretch,MyGUI::SubSkin)
SB_META_DECLARE_OBJECT(Sandbox::mygui::SubSkinHalfVStretch,MyGUI::SubSkin)
SB_META_DECLARE_OBJECT(Sandbox::mygui::SubSkinHalfStateInfo,MyGUI::SubSkinStateInfo)


namespace Sandbox {
    namespace mygui {
        SubSkinHalfHStretch::SubSkinHalfHStretch() : m_center(0) {
            
        }
        
        void SubSkinHalfHStretch::setStateData(MyGUI::IStateInfo* _data) {
            m_center = _data->castType<SubSkinHalfStateInfo>()->getCenter();
            Base::setStateData(_data);
        }
        
        void SubSkinHalfHStretch::_setAlign(const MyGUI::IntSize& _oldsize) {
            // необходимо разобраться
            bool need_update = true;//_update;
            
            // растягиваем
            if (mAlign.isLeft()) {
                mCoord.width += (mCroppedParent->getWidth()-m_center)/2 - (_oldsize.width-m_center) / 2;
                need_update = true;
                mIsMargin = true; // при изменении размеров все пересчитывать
            } else if (mAlign.isRight()) {
                
                int dleft = (mCroppedParent->getWidth()-m_center)/2 - (_oldsize.width-m_center) / 2;
                int dwidth = mCroppedParent->getWidth() - _oldsize.width;
                
                mCoord.left += dleft;
                mCoord.width += dwidth - dleft;
                need_update = true;
                mIsMargin = true; // при изменении размеров все пересчитывать
            } else {
                MYGUI_ASSERT(false, "HAlign must be left or right");
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
        
        
        SubSkinHalfVStretch::SubSkinHalfVStretch() : m_center(0) {
            
        }
        
        void SubSkinHalfVStretch::setStateData(MyGUI::IStateInfo* _data) {
            m_center = _data->castType<SubSkinHalfStateInfo>()->getCenter();
            Base::setStateData(_data);
        }
        
        void SubSkinHalfVStretch::_setAlign(const MyGUI::IntSize& _oldsize) {
            // необходимо разобраться
            bool need_update = true;//_update;
            
            // растягиваем
            if (mAlign.isTop()) {
                mCoord.height += (mCroppedParent->getHeight()-m_center)/2 - (_oldsize.height - m_center) / 2;
                need_update = true;
                mIsMargin = true; // при изменении размеров все пересчитывать
            } else if (mAlign.isBottom()) {
                int dtop = (mCroppedParent->getHeight()-m_center)/2 - (_oldsize.height - m_center) / 2;
                int dheight = mCroppedParent->getHeight() - _oldsize.height;
                mCoord.top += dtop;
                mCoord.height += dheight - dtop;
                need_update = true;
                mIsMargin = true; // при изменении размеров все пересчитывать
            } else {
                MYGUI_ASSERT(false, "HAlign must be left or right");
            }
            
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
            
            if (need_update)
            {
                mCurrentCoord = mCoord;
                _updateView();
            }
            
        }
    }
}
