//
//  sb_gui_decorator.h
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/8/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#ifndef __sr_osx__sb_gui_decorator__
#define __sr_osx__sb_gui_decorator__

#include <sbstd/sb_shared_ptr.h>
#include <sbstd/sb_vector.h>

namespace Sandbox {
    
    class Graphics;
    
    namespace Gui {
        class Widget;
        
        class Decorator {
        public:
            virtual ~Decorator() {}
            virtual void Draw(Graphics& g,const Widget* w) const = 0;
        };
        typedef sb::shared_ptr<Decorator> DecoratorPtr;
        
        class WidgetSkin {
        public:
            virtual ~WidgetSkin() {}
            virtual void Draw(Graphics& g,const Widget* w) const;
            void AddDecorator(const DecoratorPtr& d);
        private:
            typedef sb::vector<DecoratorPtr>    DecoratorsList;
            DecoratorsList  m_decorators;
        };
        typedef sb::shared_ptr<WidgetSkin> WidgetSkinPtr;
    }
}

#endif /* defined(__sr_osx__sb_gui_decorator__) */
