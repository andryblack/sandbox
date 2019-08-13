/*
 *  sb_container_base.h
 *  Copyright 2019 andryblack. All rights reserved.
 *
 */

#ifndef SB_CONTAINER_BASE_H
#define SB_CONTAINER_BASE_H

#include "sb_scene_object.h"
#include <sbstd/sb_vector.h>

namespace Sandbox {
    
    class ContainerBase : public SceneObject {
        SB_META_OBJECT
    public:
        ContainerBase();
        ~ContainerBase();
        
        void Reserve(size_t size);
        virtual void Draw(Graphics& g) const SB_OVERRIDE;
        void AddObject(const SceneObjectPtr& o);
        void RemoveObject(const SceneObjectPtr& obj);
        virtual void Clear();
        
        void Update( float dt ) SB_OVERRIDE;
        
        void SortByOrder();
    protected:
        void UpdateChilds( float dt );
        sb::vector<SceneObjectPtr> m_objects;
        virtual void DrawChilds( Graphics& g ) const;
    private:
        sb::vector<SceneObjectPtr> m_update_objects;
    };
    typedef sb::intrusive_ptr<ContainerBase> ContainerBasePtr;
}

#endif /*SB_CONTAINER_BASE_H*/

