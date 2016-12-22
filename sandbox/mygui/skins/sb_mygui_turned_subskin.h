#ifndef MYGUI_TURNED_SUBSKIN_H_
#define MYGUI_TURNED_SUBSKIN_H_

#include "MyGUI_CommonStateInfo.h"
#include "MyGUI_SubSkin.h"

namespace Sandbox {

    namespace mygui {

        class TurnedSubSkinStateInfo : public MyGUI::SubSkinStateInfo {
            MYGUI_RTTI_DERIVED( TurnedSubSkinStateInfo )
        public:
            int getTurn() const { return _turn; }
            
        protected:
            virtual void deserialization(MyGUI::xml::ElementPtr _node, MyGUI::Version _version) {
                Base::deserialization(_node, _version);
                
                int turn = 0;
                std::string str = _node->findAttribute("turn");
                if (!str.empty()) {
                    turn = std::atoi(str.c_str());
                }
                setTurn(turn);
            }
            
            void setTurn(int turn) { _turn = turn; }
            
        private:
            int _turn; // count pi/2 rotation (clockwise)
        };

        class TurnedSubSkin : public MyGUI::SubSkin {
            MYGUI_RTTI_DERIVED( TurnedSubSkin )
        public:
            TurnedSubSkin();
            
            virtual void doRender(MyGUI::IRenderTarget* _target);
            
            virtual void setStateData(MyGUI::IStateInfo* _data);
            
            /*internal:*/
            virtual void _updateView();
            virtual void _setUVSet(const MyGUI::FloatRect& _rect);
            
        private:
            void setupUV();
            void fillQuad(MyGUI::VertexQuad& _quad, MyGUI::IRenderTarget* _target);
            
            int mTurn;
        };


    }

}

#endif /* MYGUI_TURNED_SUBSKIN_H_ */
