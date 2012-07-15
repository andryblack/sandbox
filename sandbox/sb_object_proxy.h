/*
 *  sb_object_proxy.h
 *  SR
 *
 *  Created by Андрей Куницын on 23.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_OBJECT_PROXY_H
#define SB_OBJECT_PROXY_H

#include "sb_scene_object.h"
#include "sb_function.h"
#include "sb_vector2.h"

namespace Sandbox {
	
	class ObjectDrawProxy : public SceneObject {
        SB_META_OBJECT
    public:
  
		explicit ObjectDrawProxy(const sb::function<void(Graphics&)>& func);
		void Draw(Graphics& g) const;
        
  	private:
		sb::function<void(Graphics&)> m_func;
	};
	typedef sb::shared_ptr<ObjectDrawProxy> ObjectDrawProxyPtr;
    
    
    class ObjectTouchProxy : public SceneObject {
        SB_META_OBJECT
    public:
        typedef sb::function<bool(TouchInfo)> TouchFunc;
        void Draw(Graphics&) const {}
		explicit ObjectTouchProxy(const TouchFunc& func);
		
        /// self mouse handling implementation
        bool HandleTouch( const TouchInfo& touch );
	private:
        TouchFunc   m_touch_func;
  	};
	typedef sb::shared_ptr<ObjectTouchProxy> ObjectTouchProxyPtr;
}

#endif /*SB_OBJECT_PROXY_H*/
