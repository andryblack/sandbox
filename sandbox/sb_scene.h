/*
 *  sb_scene.h
 *  SR
 *
 *  Created by Андрей Куницын on 08.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_SCENE_H
#define SB_SCENE_H

#include "sb_container.h"
#include "sb_widget.h"

namespace Sandbox {
	
	class Graphics;
	
	class Scene : public Container {
        SB_META_OBJECT
	public:
        /// draw scene
		void Draw(Graphics& g) const;
        /// handle touch
        bool HandleTouch( const TouchInfo& touch );
        /// update
        void Update( float dt );
    private:
	    WidgetWeakPtr   m_focus_widget;
	};
}
#endif /*SB_SCENE_H*/
