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

namespace Sandbox {
    
    struct GeometryData {
        sb::vector<GHL::Vertex> vertexes;
        sb::vector<GHL::UInt16> indexes;
        TexturePtr  texture;
        GHL::PrimitiveType  primitives;
    };
    
    struct GeometryBuilder {
        static void BuildLine(GeometryData& buffer,const sb::vector<Vector2f>& points,const Image& img,const Color& color);
        static void BuildContour(GeometryData& buffer,const sb::vector<Vector2f>& points,const Image& img,const Color& color);
    };
    
}

#endif /* defined(__Sandbox__sb_geometry__) */
