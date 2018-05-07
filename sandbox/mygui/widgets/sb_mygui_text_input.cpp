#include "sb_mygui_text_input.h"
#include <ghl_system.h>

#include "../../sb_ref_cntr.h"
#include "../sb_mygui_gui.h"
#include "MyGUI_InputManager.h"
#include <ghl_event.h>

SB_META_DECLARE_OBJECT(Sandbox::mygui::TextInput, Sandbox::mygui::TextWidget)

namespace Sandbox {
    namespace mygui {
        
        MYGUI_IMPL_TYPE_NAME(TextInput)
                
        TextInput::TextInput() :  m_client(0) ,m_placeholder_widget(0), m_maxt_text_length(0){
        }
        
        TextInput::~TextInput() {
      
        }
        
        void TextInput::onEvent(GHL::Event* e) {
            if (MyGUI::InputManager::getInstance().getKeyFocusWidget()!=this)
                return;
            if (e->type == GHL::EVENT_TYPE_TEXT_INPUT_ACCEPTED) {
                setCaption(e->data.text_input_accepted.text);
                emitAccept();
            } else if (e->type == GHL::EVENT_TYPE_TEXT_INPUT_TEXT_CHANGED) {
                setCaption(e->data.text_input_text_changed.text);
                emitChanged();
            } else if (e->type == GHL::EVENT_TYPE_TEXT_INPUT_CLOSED) {
                MyGUI::InputManager::getInstance().resetKeyFocusWidget();
            }
        }

        
        void TextInput::emitAccept() {
            eventEditAccept(this);
        }
        void TextInput::emitChanged() {
            eventEditTextChange(this);
        }
        
        void TextInput::setPlaceholder(const sb::string& text) {
            m_placeholder = text;
            if (m_placeholder_widget) {
                m_placeholder_widget->setCaption(text);
            }
        }
        
        
        void TextInput::initialiseOverride() {
            Base::initialiseOverride();
            setNeedKeyFocus(true);
            assignWidget(m_client, "Client");
            if (!m_client) {
                m_client = this;
            }
            assignWidget(m_placeholder_widget, "Placeholder");
            GUI::getInstancePtr()->eventGHLEvent+=MyGUI::newDelegate( this, &TextInput::onEvent );
        }
        
        /** Set widget caption */
        void TextInput::setCaption(const MyGUI::UString& _value) {
            if (m_maxt_text_length != 0) {
                MyGUI::UString text = _value;
                MyGUI::TextIterator::cutMaxLength(text,m_maxt_text_length);
                m_client->setCaption(text);
            } else {
                m_client->setCaption(_value);
            }
            if (!_value.empty() && m_placeholder_widget) {
                m_placeholder_widget->setVisible(false);
            }
            if (_value.empty() && m_placeholder_widget) {
                m_placeholder_widget->setVisible(true);
            }
        }
        /** Get widget caption */
        const MyGUI::UString& TextInput::getCaption() const {
            return m_client->getCaption();
        }
        
        void TextInput::shutdownOverride() {
            GUI::getInstancePtr()->eventGHLEvent-=MyGUI::newDelegate( this, &TextInput::onEvent );
        }
                
        void TextInput::setPropertyOverride(const std::string& _key, const std::string& _value) {
            if (_key=="Placeholder") {
                setPlaceholder(_value);
            } else if (_key=="MaxTextLength") {
                setMaxTextLength(MyGUI::utility::parseValue<size_t>(_value));
            } else {
                Base::setPropertyOverride(_key, _value);
                return;
            }
            eventChangeProperty(this, _key, _value);
        }
        
        
        void TextInput::setMaxTextLength(size_t _value) {
            m_maxt_text_length = _value;
        }
       
        size_t TextInput::getMaxTextLength() const {
            return m_maxt_text_length;
        }
    }
}
