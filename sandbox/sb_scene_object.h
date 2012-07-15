/*
 *  sb_scene_object.h
 *
 *  Created by Андрей Куницын on 08.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_SCENE_OBJECT_H
#define SB_SCENE_OBJECT_H

#include "sb_shared_ptr.h"
#include "sb_notcopyable.h"
#include "meta/sb_meta.h"

namespace Sandbox {
	
	class Graphics;
	class Container;
    class TouchInfo;

	class SceneObject : public meta::object, public NotCopyable {
	    SB_META_OBJECT
    public:
    	SceneObject();
		virtual ~SceneObject();
		/// self drawing implementation
		virtual void Draw(Graphics& g) const = 0;
        /// self update object
        virtual void Update( float dt ) {(void)dt;}
        /// visible
		void SetVisible(bool v) { m_visible = v;}
		bool GetVisible() const { return m_visible;}
        
        /// draw
        void DoDraw(Graphics& g) const {
			if (m_visible) Draw(g);
		}
        
        /// update
        void DoUpdate( float dt ) {
            if (m_visible) Update(dt);
        }
        
        /// self mouse handling implementation
        virtual bool HandleTouch( const TouchInfo& touch );
        
        /// mouse handling
        bool DoHandleTouch( const TouchInfo& touch ) {
            if (m_visible) return HandleTouch( touch );
            return false;
        }
	protected:
		friend class Container;
		Container* GetParent() const { return m_parent;}
	private:
		void SetParent(Container* parent);
		Container* m_parent;
		bool	m_visible;
	};
	typedef sb::shared_ptr<SceneObject> SceneObjectPtr;
}

#endif /*SB_SCENE_OBJECT_H*/
