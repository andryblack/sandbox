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
#include <sbstd/sb_function.h>
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
	typedef sb::intrusive_ptr<ObjectDrawProxy> ObjectDrawProxyPtr;
    
}

#endif /*SB_OBJECT_PROXY_H*/
