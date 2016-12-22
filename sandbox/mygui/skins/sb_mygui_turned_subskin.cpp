#include "sb_mygui_turned_subskin.h"

#include "MyGUI_RenderItem.h"
#include "meta/sb_meta.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::TurnedSubSkinStateInfo, MyGUI::SubSkinStateInfo)
SB_META_DECLARE_OBJECT(Sandbox::mygui::TurnedSubSkin,MyGUI::SubSkin)

namespace Sandbox {
    
    namespace mygui {
        TurnedSubSkin::TurnedSubSkin() {
        }

        void TurnedSubSkin::setupUV() {
            float tmp[4] = {
                mMargin.left / (float)mCoord.width,
                mMargin.top / (float)mCoord.height,
                mMargin.right / (float)mCoord.width,
                mMargin.bottom / (float)mCoord.height
            };
            float UV_lft = tmp[(0+mTurn)%4];
            float UV_top = tmp[(1+mTurn)%4];
            float UV_rgt = tmp[(2+mTurn)%4];
            float UV_btm = tmp[(3+mTurn)%4];
            
            float UV_sizeX = mRectTexture.right - mRectTexture.left;
            float UV_sizeY = mRectTexture.bottom - mRectTexture.top;
            
            float UV_lft_total = mRectTexture.left + UV_lft * UV_sizeX;
            float UV_top_total = mRectTexture.top + UV_top * UV_sizeY;
            float UV_rgt_total = mRectTexture.right - UV_rgt * UV_sizeX;
            float UV_btm_total = mRectTexture.bottom - UV_btm * UV_sizeY;
            
            mCurrentTexture.set(UV_lft_total, UV_top_total, UV_rgt_total, UV_btm_total);
        }

        void TurnedSubSkin::_updateView() {
            //mAbsolutePosition = mCroppedParent->getAbsolutePosition() + mCoord.point();
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
                
                if ((mCurrentCoord.width > 0) && (mCurrentCoord.height > 0))
                {
                    setupUV();
                }
            }
            
            if (mIsMargin && !margin)
            {
                // мы не обрезаны, но были, ставим базовые координаты
                mCurrentTexture = mRectTexture;
            }
            
            // запоминаем текущее состояние
            mIsMargin = margin;
            
            if (nullptr != mNode)
                mNode->outOfDate(mRenderItem);
        }

        void TurnedSubSkin::_setUVSet(const MyGUI::FloatRect& _rect)
        {
            if (mRectTexture == _rect)
                return;
            mRectTexture = _rect;
            
            // если обрезаны, то просчитываем с учето обрезки
            if (mIsMargin)
            {
                setupUV();
            }
            // мы не обрезаны, базовые координаты
            else
            {
                mCurrentTexture = mRectTexture;
            }
            
            if (nullptr != mNode)
                mNode->outOfDate(mRenderItem);
        }

        void TurnedSubSkin::fillQuad(MyGUI::VertexQuad& _quad, MyGUI::IRenderTarget* _target) {
            float vertex_z = mNode->getNodeDepth();
            
            float vertex_left = (float)(mCurrentCoord.left + mCroppedParent->getAbsoluteLeft());
            float vertex_right = vertex_left + (float)mCurrentCoord.width;
            float vertex_top = (float)(mCurrentCoord.top + mCroppedParent->getAbsoluteTop()) ;
            float vertex_bottom = vertex_top + (float)mCurrentCoord.height;
            float uv[4*2];
            uv[0+(mTurn+0)%4] = mCurrentTexture.left;
            uv[4+(mTurn+0)%4] = mCurrentTexture.top;
            uv[0+(mTurn+1)%4] = mCurrentTexture.right;
            uv[4+(mTurn+1)%4] = mCurrentTexture.top;
            uv[0+(mTurn+2)%4] = mCurrentTexture.right;
            uv[4+(mTurn+2)%4] = mCurrentTexture.bottom;
            uv[0+(mTurn+3)%4] = mCurrentTexture.left;
            uv[4+(mTurn+3)%4] = mCurrentTexture.bottom;
            
            _quad.vertex[_quad.CornerLT].set(vertex_left,vertex_top, vertex_z, uv[0],uv[4], mCurrentColour);
            _quad.vertex[_quad.CornerRT].set(vertex_right,vertex_top, vertex_z, uv[1],uv[5], mCurrentColour);
            _quad.vertex[_quad.CornerLB].set(vertex_left,vertex_bottom, vertex_z, uv[3],uv[7], mCurrentColour);
            _quad.vertex[_quad.CornerRB].set(vertex_right,vertex_bottom, vertex_z, uv[2],uv[6], mCurrentColour);
        }

        void TurnedSubSkin::doRender(MyGUI::IRenderTarget* _target)
        {
            if (!mVisible || mEmptyView)
                return;
            
            _target->setTexture(getTexture());
            MyGUI::VertexQuad quad;
            
            fillQuad(quad, _target);
            
            _target->addQuad(quad);
        }

        void TurnedSubSkin::setStateData(MyGUI::IStateInfo* _data) {
            mTurn = _data->castType<TurnedSubSkinStateInfo>()->getTurn() % 4;
            Base::setStateData(_data);
        }

    }
}
