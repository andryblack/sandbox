//
//  sb_clear_scene.h
//  YinYang
//
//  Created by Андрей Куницын on 2/15/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#ifndef YinYang_sb_clear_scene_h
#define YinYang_sb_clear_scene_h

#include "sb_scene_object.h"
#include "sb_color.h"

namespace Sandbox {
    
    class ClearScene : public SceneObject {
        SB_META_OBJECT
    public:
        ClearScene() : m_depth(1.0) {}
        void Draw( Graphics& g ) const SB_OVERRIDE;
        
        void SetColor( const Color& c ) { m_color = c; }
        const Color& GetColor() const { return m_color; }
        
        void SetDepth(float d) { m_depth = d; }
        float GetDepth() const { return m_depth; }
    private:
        Color   m_color;
        float   m_depth;
    };
    
}


#endif
