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

#include "sb_scene_object.h"

namespace Sandbox {
	
	class Graphics;
	
	class Scene {
	public:
        /// draw scene
		void Draw(Graphics& g) const;
        /// handle touch
        bool HandleTouch( const TouchInfo& touch );
        /// update
        void Update( float dt );
        
		void SetRoot(const SceneObjectPtr& obj) { m_root = obj;}
		const SceneObjectPtr& GetRoot() const { return m_root;}
	private:
		SceneObjectPtr	m_root;
	};
}
#endif /*SB_SCENE_H*/
