/*
 *  sb_controllers_color.h
 *  SR
 *
 *  Created by Андрей Куницын on 19.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_CONTROLLERS_COLOR_H
#define SB_CONTROLLERS_COLOR_H

#include "sb_controller.h"
#include "sb_draw_modificator.h"

namespace Sandbox {
	
	namespace Impl {
		
		
		struct LinearControllerSetColor {
			static void SetValue( const ColorModificatorPtr& ctnr,const Color& c) {
				ctnr->SetColor(c);
			}
		};
		struct LinearControllerSetAlpha {
			static void SetValue( const ColorModificatorPtr& ctnr,float a) {
				ctnr->SetAlpha(a);
			}
		};
	}
	
	typedef LinearController<Color,Impl::LinearControllerSetColor,ColorModificatorPtr> ControllerColor;
	typedef LinearController<float,Impl::LinearControllerSetAlpha,ColorModificatorPtr> ControllerAlpha;
	
	
}

#endif /*SB_CONTROLLERS_COLOR_H*/
