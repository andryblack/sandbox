//
//  sb_gui_decorator.cpp
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/8/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#include "sb_gui_decorator.h"

namespace Sandbox {
    
    namespace Gui {
        
        void WidgetSkin::Draw(Graphics& g,const Widget* w) const {
            for (DecoratorsList::const_iterator it = m_decorators.begin();it!=m_decorators.end();++it) {
                (*it)->Draw(g, w);
            }
        }
        
        void WidgetSkin::AddDecorator(const DecoratorPtr& d) {
            m_decorators.push_back(d);
        }
    }
    
}

