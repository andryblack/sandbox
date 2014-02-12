//
//  sb_geometry.h
//  Sandbox
//
//  Created by Andrey Kunitsyn on 06/02/14.
//
//

#ifndef __Sandbox__sb_geometry__
#define __Sandbox__sb_geometry__

#include <sbstd/sb_vector.h>
#include <ghl_types.h>
#include <ghl_render.h>
#include "sb_color.h"
#include "sb_texture.h"
#include "sb_vector2.h"
#include "sb_matrix3.h"
#include "sb_image.h"
#include "sb_transform2d.h"

namespace Sandbox {
    
    struct GeometryData {
        sb::vector<GHL::Vertex> vertexes;
        sb::vector<GHL::UInt16> indexes;
        TexturePtr  texture;
        GHL::PrimitiveType  primitives;
    };
    
    class GeometryBuilder {
    private:
        GeometryData& buffer;
        Color   color;
    public:
        explicit GeometryBuilder(GeometryData& b) : buffer(b) {}
        void SetColor(const Color& c) { color = c; }
        void BuildLine(const sb::vector<Vector2f>& points,const Image& img);
        void BuildContour(const sb::vector<Vector2f>& points,const Image& img);
        void BuildFill(const sb::vector<Vector2f>& points,const Image& img,const Transform2d& tr);
    };
    
}

#endif /* defined(__Sandbox__sb_geometry__) */
