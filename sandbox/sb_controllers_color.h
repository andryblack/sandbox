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
#include "sb_sprite.h"
#include "sb_shader.h"

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
        struct LinearControllerSetSpriteColor {
            static void SetValue( const ColorizedSpritePtr& ctnr,const Color& c) {
                ctnr->SetColor(c);
            }
        };
        struct LinearControllerSetSpriteAlpha {
            static void SetValue( const ColorizedSpritePtr& ctnr,float a) {
                ctnr->SetAlpha(a);
            }
        };
        struct LinearControllerSetShaderColor {
            static void SetValue( const ShaderColorUniformPtr& ctnr,const Color& c) {
                ctnr->SetValue(c);
            }
        };
        struct LinearControllerSetShaderFloat {
            static void SetValue( const ShaderFloatUniformPtr& ctnr,float v) {
                ctnr->SetValue(v);
            }
        };
	}
	
	typedef LinearController<Color,Impl::LinearControllerSetColor,ColorModificatorPtr> ControllerColor;
	typedef LinearController<float,Impl::LinearControllerSetAlpha,ColorModificatorPtr> ControllerAlpha;
    typedef LinearController<Color,Impl::LinearControllerSetSpriteColor,ColorizedSpritePtr> ControllerSpriteColor;
    typedef LinearController<float,Impl::LinearControllerSetSpriteAlpha,ColorizedSpritePtr> ControllerSpriteAlpha;
    
    typedef LinearController<Color,Impl::LinearControllerSetShaderColor,ShaderColorUniformPtr> ControllerUniformColor;
    typedef LinearController<float,Impl::LinearControllerSetShaderFloat,ShaderFloatUniformPtr> ControllerUniformFloat;
    
}

#endif /*SB_CONTROLLERS_COLOR_H*/
