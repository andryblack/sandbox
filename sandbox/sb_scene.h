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

namespace Sandbox {
	
	class Graphics;
	
	class Scene : public Container {
        SB_META_OBJECT
	public:
        Scene();
        /// draw scene
		virtual void Draw(Graphics& g) const SB_OVERRIDE;
        /// update
        void Update( float dt ) SB_OVERRIDE;
        
        void SetScale( float scale );
        float GetScale() const;
    private:
        float   m_scale;
	};
    typedef sb::intrusive_ptr<Scene> ScenePtr;
}
#endif /*SB_SCENE_H*/
