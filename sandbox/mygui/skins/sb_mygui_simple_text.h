/*
 * This source file is part of MyGUI. For the latest info, see http://mygui.info/
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef MYGUI_SIMPLE_TEXT_H_
#define MYGUI_SIMPLE_TEXT_H_

#include "MyGUI_Prerequest.h"
#include "MyGUI_XmlDocument.h"
#include "MyGUI_Types.h"
#include "MyGUI_ISubWidgetText.h"
#include "sb_mygui_edit_text.h"

namespace Sandbox
{
    namespace mygui {

	class MYGUI_EXPORT SimpleText :
		public EditText
	{
		MYGUI_RTTI_DERIVED( SimpleText )

	public:
		SimpleText();
		virtual ~SimpleText();

        virtual void updateRawData();
        virtual void setViewOffset(const MyGUI::IntPoint& _point);

		virtual void doRender(MyGUI::IRenderTarget* _target);
	};
        
    }

} // namespace Sandbox

#endif // MYGUI_SIMPLE_TEXT_H_
