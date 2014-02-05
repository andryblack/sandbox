//
//  sb_geomerty_buffer.cpp
//  Sandbox
//
//  Created by Andrey Kunitsyn on 06/02/14.
//
//

#include "sb_geomerty_buffer.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::GeometryBuffer, Sandbox::SceneObject)

namespace Sandbox {
    
    GeometryBuffer::GeometryBuffer() {
        
    }
    
    GeometryBuffer::~GeometryBuffer() {
        
    }
    
    void GeometryBuffer::Draw(Graphics& g) const {
        if (!m_data.vertexes.empty()) {
            g.DrawGeometry(m_data, true);
        }
    }
    
    void GeometryBuffer::BuildLine(const sb::vector<Vector2f>& points, const ImagePtr& img, const Color& clr) {
        if (img) {
            GeometryBuilder::BuildLine(m_data, points, *img, clr);
        }
    }
    
}
