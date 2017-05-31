//
//  sb_geomerty_buffer.cpp
//  Sandbox
//
//  Created by Andrey Kunitsyn on 06/02/14.
//
//

#include "sb_geometry_buffer.h"
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
    
    void GeometryBuffer::Clear() {
        m_data.vertexes.clear();
        m_data.indexes.clear();
    }
    
    void GeometryBuffer::BuildLine(const sb::vector<Vector2f>& points) {
        if (m_image) {
            GeometryBuilder builder(m_data);
            builder.SetColor(m_color);
            builder.BuildLine( points, *m_image);
        }
    }
    void GeometryBuffer::BuildContour(const sb::vector<Vector2f>& points) {
        if (m_image) {
            GeometryBuilder builder(m_data);
            builder.SetColor(m_color);
            builder.BuildContour( points, *m_image);
        }
    }
    
    void GeometryBuffer::BuildFill(const sb::vector<Vector2f>& points, const Transform2d& tr) {
        GeometryBuilder builder(m_data);
        builder.SetColor(m_color);
        builder.BuildFill(points, m_image ? *m_image : Image(), tr);
    }
    
}
