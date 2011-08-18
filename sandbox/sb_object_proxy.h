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

#include "sb_object.h"
#include "sb_function.h"

namespace Sandbox {
	
	class ObjectProxy : public Object {
	public:
		explicit ObjectProxy(const function<void(Graphics&)>& func);
		void Draw(Graphics& g) const;
	private:
		function<void(Graphics&)> m_func;
	};
	typedef shared_ptr<ObjectProxy> ObjectProxyPtr;
}

#endif /*SB_OBJECT_PROXY_H*/
