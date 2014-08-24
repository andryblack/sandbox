/*
 *  sb_container.h
 *  SR
 *
 *  Created by Андрей Куницын on 11.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_CONTAINER_H
#define SB_CONTAINER_H

#include "sb_scene_object.h"
#include "sb_draw_modificator.h"
#include <sbstd/sb_vector.h>

namespace Sandbox {

	class Container : public SceneObject {
	    SB_META_OBJECT
    public:
    	Container();
		~Container();
		
		void Reserve(size_t size);
		virtual void Draw(Graphics& g) const;
		void AddObject(const SceneObjectPtr& o);
		void InsertBefore(const SceneObjectPtr& ob,const SceneObjectPtr& o);
        void RemoveObject(const SceneObjectPtr& obj);
		void Clear();
        
        void Update( float dt );
        
        void SetTransformModificator(const TransformModificatorPtr& ptr);
        TransformModificatorPtr GetTransformModificator();
        
        void SetColorModificator(const ColorModificatorPtr& ptr);
        ColorModificatorPtr GetColorModificator();
        
        void SetTranslate(const Vector2f& tr);
        Vector2f GetTranslate() const;
        
    protected:
    	void UpdateChilds( float dt );
		sb::vector<SceneObjectPtr> m_objects;
    private:
        void MoveToTop( SceneObject* obj );
        friend class SceneObject;
        TransformModificatorPtr     m_transform;
        ColorModificatorPtr         m_color;
	};
	typedef sb::intrusive_ptr<Container> ContainerPtr;
}

#endif /*SB_CONTAINER_H*/
