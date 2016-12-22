#ifndef MYGUI_TURNED_SUBSKIN_H_
#define MYGUI_TURNED_SUBSKIN_H_

#include "MyGUI_CommonStateInfo.h"
#include "MyGUI_SubSkin.h"

namespace Sandbox {

    namespace mygui {

        class TurnedSubSkinStateInfo : public MyGUI::SubSkinStateInfo {
            MYGUI_RTTI_DERIVED( TurnedSubSkinStateInfo )
        public:
            int getTurn() const { return m_turn; }
            bool getFlipH() const { return m_flip_h; }
            bool getFlipV() const { return m_flip_v; }
            TurnedSubSkinStateInfo() : m_turn(0),m_flip_v(false),m_flip_h(false) {}
        protected:
            virtual void deserialization(MyGUI::xml::ElementPtr _node, MyGUI::Version _version) {
                Base::deserialization(_node, _version);
                
                int turn = 0;
                std::string str = _node->findAttribute("turn");
                if (!str.empty()) {
                    turn = std::atoi(str.c_str());
                }
                setTurn(turn);
                str = _node->findAttribute("flip");
                if (str.find("v")!=str.npos) {
                    m_flip_v = true;
                } else {
                    m_flip_v = false;
                }
                if (str.find("h")!=str.npos) {
                    m_flip_h = true;
                } else {
                    m_flip_h = false;
                }
            }
            
            void setTurn(int turn) { m_turn = turn; }
            
        private:
            int m_turn; // count pi/2 rotation (clockwise)
            bool m_flip_v;
            bool m_flip_h;
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
            
            int m_turn;
            bool m_flip_h;
            bool m_flip_v;
        };


    }

}

#endif /* MYGUI_TURNED_SUBSKIN_H_ */
