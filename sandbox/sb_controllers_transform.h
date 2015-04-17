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
#include "sb_draw_modificator.h"
#include "sb_scene_object.h"

namespace Sandbox {
	
	namespace Impl {
		
	
		struct LinearControllerSetTranslate {
			static void SetValue( const TransformModificatorPtr& tr,const Vector2f& pos) {
				tr->SetTranslate(pos);
			}
		};
        struct LinearControllerSetPos {
            static void SetValue( const SceneObjectWithPositionPtr& tr,const Vector2f& pos) {
                tr->SetPos(pos);
            }
        };
		struct LinearControllerSetScale {
			static void SetValue( const TransformModificatorPtr& tr,float s) {
				tr->SetScale(s);
			}
		};
		struct LinearControllerSetScaleX {
			static void SetValue( const TransformModificatorPtr& tr,float s) {
				tr->SetScaleX(s);
			}
		};
		struct LinearControllerSetScaleY {
			static void SetValue( const TransformModificatorPtr& tr,float s) {
				tr->SetScaleY(s);
			}
		};
		struct LinearControllerSetAngle {
			static void SetValue( const TransformModificatorPtr& tr,float a) {
				tr->SetAngle(a);
			}
		};
	}
	
	typedef LinearController<Vector2f,Impl::LinearControllerSetTranslate,TransformModificatorPtr> ControllerTranslate;
    typedef LinearController<Vector2f,Impl::LinearControllerSetPos,SceneObjectWithPositionPtr> ControllerPos;
	typedef LinearController<float,Impl::LinearControllerSetScale,TransformModificatorPtr> ControllerScale;
	typedef LinearController<float,Impl::LinearControllerSetScaleX,TransformModificatorPtr> ControllerScaleX;
	typedef LinearController<float,Impl::LinearControllerSetScaleY,TransformModificatorPtr> ControllerScaleY;
	typedef LinearController<float,Impl::LinearControllerSetAngle,TransformModificatorPtr> ControllerAngle;
}

#endif /*SB_CONTROLLERS_TRANSFORM_H*/
