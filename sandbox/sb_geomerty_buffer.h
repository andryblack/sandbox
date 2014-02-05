//
//  sb_geomerty_buffer.h
//  Sandbox
//
//  Created by Andrey Kunitsyn on 06/02/14.
//
//

#ifndef __Sandbox__sb_geomerty_buffer__
#define __Sandbox__sb_geomerty_buffer__

#include "sb_scene_object.h"
#include <sbstd/sb_vector.h>
#include "sb_vector2.h"
#include "sb_geometry.h"

namespace Sandbox {
    
    
    class GeometryBuffer : public SceneObject {
        SB_META_OBJECT
    public:
        GeometryBuffer();
        ~GeometryBuffer();
        virtual void Draw(Graphics& g) const;
        
        void BuildLine(const sb::vector<Vector2f>& points, const ImagePtr& img, const Color& clr);
        
        
    private:
        GeometryData  m_data;
    };
    
}

#endif /* defined(__Sandbox__sb_geomerty_buffer__) */
