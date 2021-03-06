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
#include "sb_transform_components.h"
#include "sb_scene_object.h"

namespace Sandbox {
	
	namespace Impl {
		
	
		struct LinearControllerSetTranslate {
			static void SetValue( const TransformComponentsPtr& tr,const Vector2f& pos) {
				tr->SetTranslate(pos);
			}
		};
        struct LinearControllerSetTranslateX {
            static void SetValue( const TransformComponentsPtr& tr,float x) {
                tr->SetTranslateX(x);
            }
        };
        struct LinearControllerSetTranslateY {
            static void SetValue( const TransformComponentsPtr& tr,float y) {
                tr->SetTranslateY(y);
            }
        };
        struct LinearControllerSetPos {
            static void SetValue( const SceneObjectWithPositionPtr& tr,const Vector2f& pos) {
                tr->SetPos(pos);
            }
        };
		struct LinearControllerSetScale {
			static void SetValue( const TransformComponentsPtr& tr,float s) {
				tr->SetScale(s);
			}
		};
		struct LinearControllerSetScaleX {
			static void SetValue( const TransformComponentsPtr& tr,float s) {
				tr->SetScaleX(s);
			}
		};
		struct LinearControllerSetScaleY {
			static void SetValue( const TransformComponentsPtr& tr,float s) {
				tr->SetScaleY(s);
			}
		};
        struct LinearControllerSetScaleXY {
            static void SetValue( const TransformComponentsPtr& tr,const Vector2f& s) {
                tr->SetScaleX(s.x);
                tr->SetScaleY(s.y);
            }
        };
		struct LinearControllerSetAngle {
			static void SetValue( const TransformComponentsPtr& tr,float a) {
				tr->SetAngle(a);
			}
		};
	}
	
	typedef LinearController<Vector2f,Impl::LinearControllerSetTranslate,TransformComponentsPtr> ControllerTranslate;
    typedef LinearController<float,Impl::LinearControllerSetTranslateX,TransformComponentsPtr> ControllerTranslateX;
    typedef LinearController<float,Impl::LinearControllerSetTranslateY,TransformComponentsPtr> ControllerTranslateY;
    typedef MoveController<Impl::LinearControllerSetTranslate,TransformComponentsPtr> ControllerTranslateMove;
    typedef LinearController<Vector2f,Impl::LinearControllerSetPos,SceneObjectWithPositionPtr> ControllerPos;
    typedef MoveController<Impl::LinearControllerSetPos,SceneObjectWithPositionPtr> ControllerPosMove;
	typedef LinearController<float,Impl::LinearControllerSetScale,TransformComponentsPtr> ControllerScale;
	typedef LinearController<float,Impl::LinearControllerSetScaleX,TransformComponentsPtr> ControllerScaleX;
	typedef LinearController<float,Impl::LinearControllerSetScaleY,TransformComponentsPtr> ControllerScaleY;
    typedef LinearController<Vector2f,Impl::LinearControllerSetScaleXY,TransformComponentsPtr> ControllerScaleXY;
	typedef LinearController<float,Impl::LinearControllerSetAngle,TransformComponentsPtr> ControllerAngle;
}

#endif /*SB_CONTROLLERS_TRANSFORM_H*/
