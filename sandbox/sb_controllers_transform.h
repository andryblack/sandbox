/*
 *  sb_controllers_transform.h
 *  SR
 *
 *  Created by Андрей Куницын on 15.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_CONTROLLERS_TRANSFORM_H
#define SB_CONTROLLERS_TRANSFORM_H

#include "sb_controller.h"
#include "sb_container_transform.h"

namespace Sandbox {
	
	namespace Impl {
		
	
		struct LinearControllerSetTranslate {
			static void SetValue( const ContainerTransformPtr& tr,const Vector2f& pos) {
				tr->SetTranslate(pos);
			}
		};
		struct LinearControllerSetScale {
			static void SetValue( const ContainerTransformPtr& tr,float s) {
				tr->SetScale(s);
			}
		};
		struct LinearControllerSetScaleX {
			static void SetValue( const ContainerTransformPtr& tr,float s) {
				tr->SetScaleX(s);
			}
		};
		struct LinearControllerSetScaleY {
			static void SetValue( const ContainerTransformPtr& tr,float s) {
				tr->SetScaleY(s);
			}
		};
		struct LinearControllerSetAngle {
			static void SetValue( const ContainerTransformPtr& tr,float a) {
				tr->SetAngle(a);
			}
		};
	}
	
	typedef LinearController<Vector2f,Impl::LinearControllerSetTranslate,ContainerTransformPtr> ControllerTranslate;
	typedef LinearController<float,Impl::LinearControllerSetScale,ContainerTransformPtr> ControllerScale;
	typedef LinearController<float,Impl::LinearControllerSetScaleX,ContainerTransformPtr> ControllerScaleX;
	typedef LinearController<float,Impl::LinearControllerSetScaleY,ContainerTransformPtr> ControllerScaleY;
	typedef LinearController<float,Impl::LinearControllerSetAngle,ContainerTransformPtr> ControllerAngle;
}

#endif /*SB_CONTROLLERS_TRANSFORM_H*/