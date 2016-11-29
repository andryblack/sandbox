#include "sb_mygui_half_stretch.h"
#include "meta/sb_meta.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::SubSkinHalfHStretch,MyGUI::SubSkin)

namespace Sandbox {
    namespace mygui {
        SubSkinHalfHStretch::SubSkinHalfHStretch() {
            
        }
        
        void SubSkinHalfHStretch::_setAlign(const MyGUI::IntSize& _oldsize) {
            // необходимо разобраться
            bool need_update = true;//_update;
            
            // растягиваем
            if (mAlign.isLeft()) {
                mCoord.width += (mCroppedParent->getWidth() - _oldsize.width) / 2;
                need_update = true;
                mIsMargin = true; // при изменении размеров все пересчитывать
            } else if (mAlign.isRight()) {
                mCoord.left += (mCroppedParent->getWidth() - _oldsize.width) / 2;
                mCoord.width += (mCroppedParent->getWidth() - _oldsize.width) / 2;
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
    }
}
