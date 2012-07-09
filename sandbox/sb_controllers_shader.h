/*
 *  sb_controllers_shader.h
 *  SR
 *
 *  Created by Андрей Куницын on 19.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_CONTROLLERS_SHADER_H
#define SB_CONTROLLERS_SHADER_H

#include "sb_controller.h"
#include "sb_shader.h"

namespace Sandbox {

	namespace Impl {
		
		
		struct LinearControllerSetUniformFloat {
			static void SetValue( const sb::shared_ptr<ShaderFloatUniform>& tr,float s) {
				tr->SetValue(s);
			}
		};
	}
	
	typedef LinearController<float,Impl::LinearControllerSetUniformFloat,sb::shared_ptr<ShaderFloatUniform> > ControllerShaderFloatUniform;
}

#endif /*SB_CONTROLLERS_SHADER_H*/
