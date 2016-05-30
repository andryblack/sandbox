#include "sb_mygui_skin.h"
#include "MyGUI_FactoryManager.h"
#include "MyGUI_SubWidgetManager.h"
#include "meta/sb_meta.h"
#include "MyGUI_RenderItem.h"
#include "MyGUI_ITexture.h"
#include "widgets/sb_mygui_mask_image.h"
#include "widgets/sb_mygui_scene_object.h"

SB_META_DECLARE_OBJECT(Sandbox::mygui::ColorizedSubSkinStateInfo,MyGUI::SubSkinStateInfo)

SB_META_DECLARE_OBJECT(Sandbox::mygui::ColorizedSubSkin,MyGUI::SubSkin)
SB_META_DECLARE_OBJECT(Sandbox::mygui::CopySubSkin,MyGUI::SubSkin)

namespace Sandbox {

    namespace mygui {
        
     
        void ColorizedSubSkin::applyColor() {
            MyGUI::Colour val(m_color.red*m_state_color.red,
                              m_color.green*m_state_color.green,
                              m_color.blue*m_state_color.blue,
                              m_color.alpha*m_state_color.alpha);
            MyGUI::uint32 colour = MyGUI::texture_utility::toColourARGB(val);
            mCurrentColour = (colour & 0x00FFFFFF) | (mCurrentColour & 0xFF000000);
            MyGUI::texture_utility::convertColour(mCurrentColour, mVertexFormat);
        }
        void ColorizedSubSkin::_setColour(const MyGUI::Colour& _value) {
            m_color = _value;
            applyColor();
            if (nullptr != mNode)
                mNode->outOfDate(mRenderItem);
        }
        void ColorizedSubSkin::setStateData(MyGUI::IStateInfo* _data) {
            MyGUI::SubSkin::setStateData(_data);
            m_state_color = _data->castType<ColorizedSubSkinStateInfo>()->getColor();
            
            applyColor();
            
            if (nullptr != mNode)
                mNode->outOfDate(mRenderItem);
        }
        
        void CopySubSkin::_setUVSet(const MyGUI::FloatRect &_rect) {
            MyGUI::FloatRect r;
            if (!mRenderItem) return;
            MyGUI::ITexture* tex = mRenderItem->getTexture();
            if (tex) {
                int left = mCoord.left;
                int top = mCoord.top;
                r.set(float(left)/tex->getWidth(),
                      float(top)/tex->getHeight(),
                      float(left+mCoord.width)/tex->getWidth(),
                      float(top+mCoord.height)/tex->getHeight());
                Base::_setUVSet(r);
            }
            
        }
        
        void CopySubSkin::_updateView() {
            Base::_updateView();
            _setUVSet(MyGUI::FloatRect());
        }
        
        void CopySubSkin::createDrawItem(MyGUI::ITexture* _texture, MyGUI::ILayerNode* _node) {
            Base::createDrawItem(_texture, _node);
            _setUVSet(MyGUI::FloatRect());
        }
        
        
        void CopySubSkin::doRender() {
            Base::doRender();
            if (!mVisible || mEmptyView)
                return;
            MyGUI::VertexQuad* quad = reinterpret_cast<MyGUI::VertexQuad*>(mRenderItem->getCurrentVertexBuffer());
            // use premultiplied
            if (mAlign.isLeft()) {
                quad->vertex[MyGUI::VertexQuad::CornerLB].colour =
                quad->vertex[MyGUI::VertexQuad::CornerLB2].colour =
                quad->vertex[MyGUI::VertexQuad::CornerLT].colour = 0x00000000;
            }
            if (mAlign.isRight()) {
                quad->vertex[MyGUI::VertexQuad::CornerRB].colour =
                quad->vertex[MyGUI::VertexQuad::CornerRT].colour =
                quad->vertex[MyGUI::VertexQuad::CornerRT2].colour = 0x00000000;
            }
            if (mAlign.isTop()) {
                quad->vertex[MyGUI::VertexQuad::CornerLT].colour =
                quad->vertex[MyGUI::VertexQuad::CornerRT].colour =
                quad->vertex[MyGUI::VertexQuad::CornerRT2].colour = 0x00000000;
            }
            if (mAlign.isBottom()) {
                quad->vertex[MyGUI::VertexQuad::CornerLB].colour =
                quad->vertex[MyGUI::VertexQuad::CornerRB].colour =
                quad->vertex[MyGUI::VertexQuad::CornerLB2].colour = 0x00000000;
            }
        }
        
        void register_skin() {
            MyGUI::FactoryManager& factory = MyGUI::FactoryManager::getInstance();
            
            factory.registerFactory<ColorizedSubSkinStateInfo>(MyGUI::SubWidgetManager::getInstance().getStateCategoryName(), "ColorizedSubSkin");
                        
            factory.registerFactory<ColorizedSubSkin>(MyGUI::SubWidgetManager::getInstance().getCategoryName());
            factory.registerFactory<CopySubSkin>(MyGUI::SubWidgetManager::getInstance().getCategoryName());
            factory.registerFactory<MaskSubSkin>(MyGUI::SubWidgetManager::getInstance().getCategoryName());
            factory.registerFactory<MaskSetSubSkin>(MyGUI::SubWidgetManager::getInstance().getCategoryName());
            factory.registerFactory<ObjectSubSkin>(MyGUI::SubWidgetManager::getInstance().getCategoryName());
            
            factory.registerFactory<MyGUI::SubSkinStateInfo>(MyGUI::SubWidgetManager::getInstance().getStateCategoryName(), "MaskSubSkin");
            factory.registerFactory<MaskSetSubSkinState>(MyGUI::SubWidgetManager::getInstance().getStateCategoryName(), "MaskSetSubSkin");
            factory.registerFactory<MyGUI::SubSkinStateInfo>(MyGUI::SubWidgetManager::getInstance().getStateCategoryName(), "ObjectSubSkin");
        }
        
        void unregister_skin() {
            MyGUI::FactoryManager& factory = MyGUI::FactoryManager::getInstance();
            factory.unregisterFactory(MyGUI::SubWidgetManager::getInstance().getStateCategoryName(), "ColorizedSubSkin");
            factory.unregisterFactory<ColorizedSubSkin>(MyGUI::SubWidgetManager::getInstance().getCategoryName());
            factory.unregisterFactory<CopySubSkin>(MyGUI::SubWidgetManager::getInstance().getCategoryName());
            factory.unregisterFactory<MaskSubSkin>(MyGUI::SubWidgetManager::getInstance().getCategoryName());
            factory.unregisterFactory<MaskSetSubSkin>(MyGUI::SubWidgetManager::getInstance().getCategoryName());
            factory.unregisterFactory<ObjectSubSkin>(MyGUI::SubWidgetManager::getInstance().getCategoryName());
            factory.unregisterFactory(MyGUI::SubWidgetManager::getInstance().getStateCategoryName(), "MaskSubSkin");
            factory.unregisterFactory(MyGUI::SubWidgetManager::getInstance().getStateCategoryName(), "MaskSetSubSkin");
            factory.unregisterFactory(MyGUI::SubWidgetManager::getInstance().getStateCategoryName(), "ObjectSubSkin");
        }
        
    }
    
}
