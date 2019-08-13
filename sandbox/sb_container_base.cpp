/*
 *  sb_container_base.cpp
 *
 *  Copyright 2019 andryblack. All rights reserved.
 *
 */

#include "sb_container_base.h"
#include "sb_graphics.h"
#include <algorithm>

SB_META_DECLARE_OBJECT(Sandbox::ContainerBase, Sandbox::SceneObject)

namespace Sandbox {
    
    ContainerBase::ContainerBase() {
        
    }
    
    ContainerBase::~ContainerBase() {
        for (std::vector<SceneObjectPtr>::iterator i = m_objects.begin();i!=m_objects.end();i++) {
            (*i)->SetParent(0);
        }
    }
    void ContainerBase::Reserve(size_t size) {
        m_objects.reserve(size);
    }
    void ContainerBase::AddObject(const SceneObjectPtr& o) {
        sb_assert(o && "null object");
        if (ContainerBase* c=o->GetParent()) {
            c->RemoveObject(o);
        }
        o->SetParent(this);
        float order = o->GetOrder();
        for (std::vector<SceneObjectPtr>::iterator it = m_objects.begin();it!=m_objects.end();++it) {
            if ((*it)->GetOrder() > order ) {
                m_objects.insert(it, o);
                return;
            }
        }
        m_objects.push_back(o);
    }
    
    static bool sort_by_order_pred( const SceneObjectPtr& a, const SceneObjectPtr& b) {
        return a->GetOrder() < b->GetOrder();
    }
    
    void ContainerBase::SortByOrder() {
        std::sort(m_objects.begin(), m_objects.end(), sort_by_order_pred);
    }
    
    void ContainerBase::RemoveObject(const SceneObjectPtr& obj) {
        sb_assert( obj && "null object");
        std::vector<SceneObjectPtr>::iterator i = std::find(m_objects.begin(),m_objects.end(),obj);
        if (i!=m_objects.end()) {
            (*i)->SetParent(0);
            m_objects.erase(i);
        }
    }
    void ContainerBase::Clear() {
        for (size_t i=0;i<m_objects.size();i++) {
            m_objects[i]->SetParent(0);
        }
        m_objects.clear();
    }
    
    void ContainerBase::DrawChilds( Graphics& g ) const {
        for (std::vector<SceneObjectPtr>::const_iterator i = m_objects.begin();i!=m_objects.end();++i) {
            if ((*i)->GetVisible()) (*i)->Draw(g);
        }
    }
    
    
    void ContainerBase::Draw(Sandbox::Graphics &g) const {
        DrawChilds(g);
    }
    
    
    void ContainerBase::Update( float dt ) {
        UpdateChilds(dt);
    }
    
    void ContainerBase::UpdateChilds( float dt ) {
        /// copy, for allow scene modifications
        m_update_objects = m_objects; // prevent per-frame allocations
        for (std::vector<SceneObjectPtr>::reverse_iterator i = m_update_objects.rbegin();i!=m_update_objects.rend();++i) {
            if ((*i)->GetVisible()) (*i)->Update(dt);
        }
        m_update_objects.clear();
    }
    
}

